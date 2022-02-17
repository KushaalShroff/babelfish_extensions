DROP VIEW IF EXISTS my_view;
GO

DROP TABLE IF EXISTS t1;
GO

DROP SEQUENCE IF EXISTS seq_tinyint;
GO

DROP FUNCTION IF EXISTS my_func;
GO

DROP PROCEDURE IF EXISTS my_proc;
GO

DROP TYPE IF EXISTS type_int;
GO

---
---  Prepare Objects
---

---- TABLE
CREATE TABLE t1 ( a int, b int);
GO

INSERT INTO t1 VALUES (1,2);
GO

---- SEQUENCE
CREATE SEQUENCE seq_tinyint
AS [tinyint]
START WITH 1
INCREMENT BY 1
CACHE  50
GO

---- VIEW
CREATE VIEW my_view AS SELECT * FROM t1;
GO

--- FUNCTION
CREATE FUNCTION my_func() RETURNS INT AS BEGIN RETURN 1; END;
GO

--- STORED PROCEDURE
CREATE PROCEDURE my_proc AS BEGIN SELECT 111; END;
GO

--- TYPE
CREATE TYPE type_int FROM INT NOT NULL;
GO

---
---  Basic Grant / Revoke
---

GRANT ALL ON OBJECT::t1 TO guest WITH GRANT OPTION;
GO

GRANT ALL ON OBJECT::seq_tinyint TO guest WITH GRANT OPTION;
GO

GRANT ALL ON OBJECT::my_view TO guest WITH GRANT OPTION;
GO

GRANT ALL ON OBJECT::my_func TO guest WITH GRANT OPTION;
GO

GRANT ALL ON OBJECT::my_proc TO guest WITH GRANT OPTION;
GO

GRANT ALL ON OBJECT::type_int TO guest WITH GRANT OPTION;
GO

REVOKE ALL ON OBJECT::t1 TO guest;
GO

REVOKE ALL ON OBJECT::seq_tinyint TO guest;
GO

REVOKE ALL ON OBJECT::my_view TO guest;
GO

REVOKE ALL ON OBJECT::my_func TO guest;
GO

REVOKE ALL ON OBJECT::my_proc TO guest;
GO

REVOKE ALL ON OBJECT::type_int TO guest;
GO

REVOKE ALL ON OBJECT::my_func FROM PUBLIC;
GO

REVOKE ALL ON OBJECT::my_proc FROM PUBLIC;
GO

REVOKE ALL ON OBJECT::type_int FROM PUBLIC;
GO

--- 
---  Unsupported cases
---
GRANT ALL TO alogin;  -- database permission
GO

REVOKE ALL TO alogin; -- database permission
GO

GRANT SHOWPLAN ON OBJECT::t1 TO guest;  -- unsupported permission
GO

REVOKE SHOWPLAN ON OBJECT::t2 TO alogin;  -- unsupported permission
GO

GRANT ALL ON SCHEMA::scm TO guest;  -- unsupported class
GO

REVOKE ALL ON SCHEMA::scm TO guest; -- unsupported class
GO

GRANT ALL ON OBJECT::t1 TO guest WITH GRANT OPTION AS superuser;
GO

REVOKE ALL ON OBJECT::t1 TO guest AS superuser;
GO

---
---  Clean Up
---

DROP VIEW IF EXISTS my_view;
GO

DROP TABLE IF EXISTS t1;
GO

DROP SEQUENCE IF EXISTS seq_tinyint;
GO

DROP FUNCTION IF EXISTS my_func;
GO

DROP PROCEDURE IF EXISTS my_proc;
GO

DROP TYPE IF EXISTS type_int;
GO