﻿/*
    Based on the original YahooStockEngine.cs by Jarloo:
 
    Jarloo
    http://jarloo.com
 
    This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License  
    http://creativecommons.org/licenses/by-sa/3.0/     

*/

namespace Stocks.ServiceClients.Yahoo
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Linq;
    using System.Xml.Linq;

    using Stocks.ServiceClients.Yahoo.ObjectModel;
    using System.Collections.Concurrent;
    using System.Threading.Tasks;
    public class YahooClient
    {
        private const string BaseUrl = "http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.quotes%20where%20symbol%20in%20({0})&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";

        public static IEnumerable<Quote> GetQuotes(IEnumerable<string> symbols)
        {
            string symbolList = String.Join("%2C", symbols.Select(s => "%22" + s + "%22").ToArray());
            string url = string.Format(BaseUrl,symbolList);
            
            XDocument doc = XDocument.Load(url);

            Debug.Assert(doc.Root != null, "doc.Root != null");

            var results = doc.Root.Element("results").Elements("quote");

            var quotes = new ConcurrentBag<Quote>();
            Parallel.ForEach(results, q =>
           //foreach(var q in results)
           {
               Debug.Assert(q != null, "q != null");

               quotes.Add(
                   new Quote
                   {
                       Symbol = q.Element("Symbol").Value,
                       Ask = GetDecimal(q.Element("Ask").Value),
                       Bid = GetDecimal(q.Element("Bid").Value),
                       AverageDailyVolume = GetDecimal(q.Element("AverageDailyVolume").Value),
                       BookValue = GetDecimal(q.Element("BookValue").Value),
                       Change = GetDecimal(q.Element("Change").Value),
                       DividendShare = GetDecimal(q.Element("DividendShare").Value),
                       LastTradeDate = GetDateTime(q.Element("LastTradeDate") + " " + q.Element("LastTradeTime").Value),
                       EarningsShare = GetDecimal(q.Element("EarningsShare").Value),
                       EpsEstimateCurrentYear = GetDecimal(q.Element("EPSEstimateCurrentYear").Value),
                       EpsEstimateNextYear = GetDecimal(q.Element("EPSEstimateNextYear").Value),
                       EpsEstimateNextQuarter = GetDecimal(q.Element("EPSEstimateNextQuarter").Value),
                       DailyLow = GetDecimal(q.Element("DaysLow").Value),
                       DailyHigh = GetDecimal(q.Element("DaysHigh").Value),
                       YearlyLow = GetDecimal(q.Element("YearLow").Value),
                       YearlyHigh = GetDecimal(q.Element("YearHigh").Value),
                       MarketCapitalization = GetDecimal(q.Element("MarketCapitalization").Value),
                       Ebitda = GetDecimal(q.Element("EBITDA").Value),
                       ChangeFromYearLow = GetDecimal(q.Element("ChangeFromYearLow").Value),
                       PercentChangeFromYearLow = GetDecimal(q.Element("PercentChangeFromYearLow").Value),
                       ChangeFromYearHigh = GetDecimal(q.Element("ChangeFromYearHigh").Value),
                       LastTradePrice = GetDecimal(q.Element("LastTradePriceOnly").Value),
                       PercentChangeFromYearHigh = GetDecimal(q.Element("PercebtChangeFromYearHigh").Value), //missspelling in yahoo for field name
                       FiftyDayMovingAverage = GetDecimal(q.Element("FiftydayMovingAverage").Value),
                       TwoHunderedDayMovingAverage = GetDecimal(q.Element("TwoHundreddayMovingAverage").Value),
                       ChangeFromTwoHundredDayMovingAverage = GetDecimal(q.Element("ChangeFromTwoHundreddayMovingAverage").Value),
                       PercentChangeFromTwoHundredDayMovingAverage = GetDecimal(q.Element("PercentChangeFromTwoHundreddayMovingAverage").Value),
                       PercentChangeFromFiftyDayMovingAverage = GetDecimal(q.Element("PercentChangeFromFiftydayMovingAverage").Value),
                       Name = q.Element("Name").Value,
                       Open = GetDecimal(q.Element("Open").Value),
                       PreviousClose = GetDecimal(q.Element("PreviousClose").Value),
                       ChangeInPercent = GetDecimal(q.Element("ChangeinPercent").Value),
                       PriceSales = GetDecimal(q.Element("PriceSales").Value),
                       PriceBook = GetDecimal(q.Element("PriceBook").Value),
                       ExDividendDate = GetDateTime(q.Element("ExDividendDate").Value),
                       PeRatio = GetDecimal(q.Element("PERatio").Value),
                       DividendPayDate = GetDateTime(q.Element("DividendPayDate").Value),
                       PegRatio = GetDecimal(q.Element("PEGRatio").Value),
                       PriceEpsEstimateCurrentYear = GetDecimal(q.Element("PriceEPSEstimateCurrentYear").Value),
                       PriceEpsEstimateNextYear = GetDecimal(q.Element("PriceEPSEstimateNextYear").Value),
                       ShortRatio = GetDecimal(q.Element("ShortRatio").Value),
                       OneYearPriceTarget = GetDecimal(q.Element("OneyrTargetPrice").Value),
                       Volume = GetDecimal(q.Element("Volume").Value),
                       StockExchange = q.Element("StockExchange").Value,
                       LastUpdate = DateTime.Now
                   }
               );
           });

            return quotes;
        }

        private static decimal? GetDecimal(string input)
        {
            if (input == null) return null;

            input = input.Replace("%", "");

            decimal value;

            if (Decimal.TryParse(input, out value)) return value;
            return null;
        }

        private static DateTime? GetDateTime(string input)
        {
            if (input == null) return null;

            DateTime value;

            if (DateTime.TryParse(input, out value)) return value;
            return null;
        }
    }
}