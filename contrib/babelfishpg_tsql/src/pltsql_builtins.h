/*-------------------------------------------------------------------------
 *
 * pltsql_builtins.h
 *	  Declarations for operations on built-in types.
 *
 *-------------------------------------------------------------------------
 */
#ifndef BUILTINS_H
#define BUILTINS_H

/* format_type.c */
extern char *tsql_format_type_extended(Oid type_oid, int32 typemod, bits16 flags);

#endif							/* BUILTINS_H */
