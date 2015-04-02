
CREATE TABLE accnt_info (
  first_name VARCHAR(50) NOT NULL,
  last_name VARCHAR(50) NOT NULL,
  accnt_no BIGINT UNSIGNED AUTO_INCREMENT,
  email VARCHAR(60),
  pin INT UNSIGNED,
  ssn INT UNSIGNED,
  DL INT UNSIGNED,
  INDEX(accnt_no), PRIMARY KEY(accnt_no)) AUTO_INCREMENT=4388576071000000;

CREATE TABLE trans_rec (
    id INT primary key not  null  auto_increment,
    accnt_no BIGINT UNSIGNED NOT NULL,
    date DATETIME not null, type SMALLINT not null,
    amount INT UNSIGNED, INDEX(accnt_no));

CREATE TABLE accnt_balance (
    accnt_no BIGINT UNSIGNED NOT NULL,
    cash_bl BIGINT UNSIGNED NOT NULL,
    check_bl BIGINT UNSIGNED NOT NULL, INDEX(accnt_no));