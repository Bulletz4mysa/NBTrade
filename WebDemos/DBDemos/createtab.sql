drop table stockdata;
create table stockdata (
	QuoteCode varchar(20),
	QuoteName varchar(80),
	Exchange varchar(20),
	HasHistory int default 0 not NULL,
	AliasCode varchar(10)
);
create unique index idxStockData on stockdata (QuoteCode,Exchange);

drop table realtime;
create table realtime (
	QuoteCode varchar(20) primary key,
	LastA float,
	OpenA float,
	HighA float,
	LowA float,
	CloseA float,
	VolumeA float,
	AMountA float,
	LastTime datetime
);
create unique index idxRealtime_Code on realtime(QuoteCode);

drop table condition;
create table condition(
	ConditionId int IDENTITY(1,1) primary key,
	Condition varchar(200),
	ConditionDesc varchar(80),
	Exchange varchar(10),
	Total int,
	Scaned int,
	ResultCount int,
	StartTime datetime,
	EndTime datetime,
	ScanType int default 0 -- 0:Custom scan , 1:Pre-scan Indicator Scan , 2:Pre-scan Pattern Scan
);

drop table ScanedQuote;
create table ScanedQuote(
	ConditionId int,
	QuoteCode varchar(20)
) on [primary];

drop table FormulaValue;
create table FormulaValue(
	QuoteCode varchar(20),
	FormulaName varchar(20),
	FormulaValue float,
	CalculateTime datetime
);
create index idxFormulaQuoteCode on FormulaValue(QuoteCode);
create index idxTime on  FormulaValue(CalculateTime);

drop table Intraday;
create table Intraday
(
	Id  int IDENTITY(1,1) primary key,
	Symbol varchar(10),
	QuoteTime datetime,
	Price float,
	Volume float
)

create index idxIntraSymbolTime on Intraday(Symbol,QuoteTime);
create index idxIntraTime on Intraday(QuoteTime);

--drop table WrongData;
--create table WrongData
--(
--	Id  int IDENTITY(1,1) primary key,
--	Symbol varchar(10),
--	LastCheckTime datetime,
--	Fixed int,
--	WrongType int, -- 1 split , 2 gap, 3 not latest
--	WrongDate datetime,
--	Result varchar(50)
--);