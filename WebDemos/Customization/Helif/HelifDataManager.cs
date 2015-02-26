using System;
using System.Data;
using Easychart.Finance.DataProvider;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// This is the DataManager for http://chart.helif.com
	/// </summary>
	public class HelifDataManager:DataManagerBase
	{
		public HelifDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			CommonDataProvider cdp = new CommonDataProvider(this);
			DataRow dr = DB.GetFirstRow("select * from CompanyInfo where TickerSymbolID='"+Code+"'");
			if (dr!=null)
			{
				cdp.SetStringData("Code",dr["Symbol"].ToString());
				cdp.SetStringData("Name",dr["CompanyName"].ToString());
				cdp.SetStringData("Exchange",dr["Exchange"].ToString());

				DataTable dt = DB.GetDataTable(
					"SELECT S.SessionNUM, S.SessionDate, Q.StockOpen, Q.StockHigh, Q.StockLow, Q.StockClose, Q.StockVolume"+
					"	FROM StockQuotes Q ,Sessions S"+
					"	WHERE Q.SessionNUM = S.SessionNUM and Q.TickerSymbolId='"+Code+"' ORDER BY Q.SessionNUM");
				double[] OPEN = new double[dt.Rows.Count];
				double[] HIGH =  new double[dt.Rows.Count];
				double[] LOW =  new double[dt.Rows.Count];
				double[] CLOSE =  new double[dt.Rows.Count];
				double[] VOLUME =  new double[dt.Rows.Count];
				double[] AMOUNT =  new double[dt.Rows.Count];
				double[] DATE = new double[dt.Rows.Count];

				for(int i=0; i<dt.Rows.Count; i++)
				{
					OPEN[i] = ToDouble(dt.Rows[i]["StockClose"]);
					HIGH[i] = ToDouble(dt.Rows[i]["StockHigh"]);
					LOW[i] = ToDouble(dt.Rows[i]["StockLow"]);
					CLOSE[i] = ToDouble(dt.Rows[i]["StockClose"]);
					VOLUME[i] = ToInt(dt.Rows[i]["StockVolume"]);
					DATE[i] = ((DateTime)dt.Rows[i]["SessionDate"]).ToOADate();
				}
				cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
				return cdp;
			}
			return base.GetData (Code, Count);
		}
	}
}