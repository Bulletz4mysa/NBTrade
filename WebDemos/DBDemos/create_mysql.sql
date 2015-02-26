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
	LastA double,
	OpenA double,
	HighA double,
	LowA double,
	CloseA double,
	VolumeA double,
	AMountA double,
	LastTime datetime
);
create unique index idxRealtime_Code on realtime(QuoteCode);

drop table condition;
create table condition(
	ConditionId int AUTO_INCREMENT primary key,
	Condition varchar(200),
	ConditionDesc varchar(80),
	Exchange varchar(20),
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
);
create unique index idxScanedQuote on ScanedQuote(ConditionId,QuoteCode);

drop table FormulaValue;
create table FormulaValue(
	QuoteCode varchar(20),
	FormulaName varchar(20),
	FormulaValue double,
	CalculateTime datetime
);
create index idxFormulaQuoteCode on FormulaValue(QuoteCode);
