create view def_v1 as select 1;
go

-- definition should not get trimmed
create view  def_v2
as
select 							2;
go

select * from babelfish_object_def order by db_name, schema_name, object_name;
go

-- create view in different schema and db
create schema def_sch1;
go

create view def_sch1.def_v1 as select 1;
go

select * from babelfish_object_def order by db_name, schema_name, object_name;
go

create database db1;
go

use db1;
go

create view db1_v1 as select 1;
go

select * from babelfish_object_def order by db_name, schema_name, object_name;
go

-- TODO: Update it when ALTER VIEW is supported
alter view def_v1 as select 2;
go

select * from babelfish_object_def order by db_name, schema_name, object_name;
go

