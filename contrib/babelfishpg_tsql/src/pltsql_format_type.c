/*-------------------------------------------------------------------------
 *
 * format_type.c
 *	  Display TSQL type names "nicely".
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"

#include "access/htup_details.h"
#include "catalog/namespace.h"
#include "catalog/pg_type.h"
#include "mb/pg_wchar.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/numeric.h"
#include "utils/syscache.h"
#include "fmgr.h"
#include "pltsql_builtins.h"

extern Datum translate_pg_type_to_tsql(PG_FUNCTION_ARGS);
static char *tsql_printTypmod(const char *typname, int32 typmod, Oid typmodout);

/*
 * tsql_format_type_extended
 *		Generate a possibly-qualified TSQL type name.
 *
 * The default behavior is to only qualify if the type is not in the search
 * path, to ignore the given typmod, and to raise an error if a non-existent
 * type_oid is given. It assumes that array type won't be supplied as input as
 * TSQL doesn't support array types.
 *
 * Refer format_type_extended() from format_type.c for flags documentation.
 *
 * Returns a palloc'd string.
 */
char *
tsql_format_type_extended(Oid type_oid, int32 typemod, bits16 flags)
{
	HeapTuple		tuple;
	Form_pg_type	typeform;
	Oid				array_base_type;
	Datum			tsql_typename;
	char		   *buf;
	bool			with_typemod;

	if (type_oid == InvalidOid && (flags & FORMAT_TYPE_ALLOW_INVALID) != 0)
		return pstrdup("-");

	tuple = SearchSysCache1(TYPEOID, ObjectIdGetDatum(type_oid));
	if (!HeapTupleIsValid(tuple))
	{
		if ((flags & FORMAT_TYPE_ALLOW_INVALID) != 0)
			return pstrdup("???");
		else
			elog(ERROR, "cache lookup failed for type %u", type_oid);
	}
	typeform = (Form_pg_type) GETSTRUCT(tuple);

	with_typemod = (flags & FORMAT_TYPE_TYPEMOD_GIVEN) != 0 && (typemod >= 0);

	buf = NULL;
	LOCAL_FCINFO(fcinfo, 1);

	InitFunctionCallInfoData(*fcinfo, NULL, 0, InvalidOid, NULL, NULL);
	fcinfo->args[0].value = ObjectIdGetDatum(type_oid);
	fcinfo->args[0].isnull = false;
	tsql_typename = (*translate_pg_type_to_tsql) (fcinfo);

	/*
	 * If it is TSQL type then report it without any qualification.
	 */
	if (tsql_typename)
		buf = text_to_cstring(DatumGetTextPP(tsql_typename));

	if (buf == NULL)
	{
		/*
		 * Default handling: report the name as it appears in the catalog.
		 * Here, we must qualify the name if it is not visible in the search
		 * path or if caller requests it; and we must double-quote it if it's
		 * not a standard identifier or if it matches any keyword.
		 */
		char	   *nspname;
		char	   *typname;

		if ((flags & FORMAT_TYPE_FORCE_QUALIFY) == 0 &&
			TypeIsVisible(type_oid))
			nspname = NULL;
		else
			nspname = get_namespace_name_or_temp(typeform->typnamespace);

		typname = NameStr(typeform->typname);

		buf = quote_qualified_identifier(nspname, typname);
	}

	if (with_typemod)
	{
		buf = tsql_printTypmod(buf, typemod, typeform->typmodout);
	}

	ReleaseSysCache(tuple);

	return buf;
}

/*
 * Add typmod decoration to the basic type name
 */
static char *
tsql_printTypmod(const char *typname, int32 typmod, Oid typmodout)
{
	char	   *res;

	/* Shouldn't be called if typmod is -1 */
	Assert(typmod >= 0);

	/*
	* In case of time, datetime2 or datetimeoffset print typmod
	* info directly because it uses timestamp typmodout function
	* which appends timezone data along with typmod which is not
	* required. Directly print typename for smalldatetime as it
	* doesn't support typmod.
	*/
	if (strcmp(typname, "time") == 0 ||
	   strcmp(typname, "datetime2") == 0 ||
	   strcmp(typname, "datetimeoffset") == 0)
	{
		res = psprintf("%s(%d)", typname, (int) typmod);
	}
	else if (strcmp(typname, "smalldatetime") == 0)
	{
		res = psprintf("%s", typname, (int) typmod);
	}
	else
	{
		if (typmodout == InvalidOid)
		{
			/* Default behavior: just print the integer typmod with parens */
			res = psprintf("%s(%d)", typname, (int) typmod);
		}
		else
		{
			/* Use the type-specific typmodout procedure */
			char	   *tmstr;

			tmstr = DatumGetCString(OidFunctionCall1(typmodout,
													 Int32GetDatum(typmod)));
			res = psprintf("%s%s", typname, tmstr);
		}
	}
	return res;
}

