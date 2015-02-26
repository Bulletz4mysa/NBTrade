using System;
using System.Data;
using Easychart.Finance.DataProvider;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// Summary description for MohdDataManager.
	/// </summary>
	public class MohdDataManager:DataManagerBase
	{
		public MohdDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			CommonDataProvider cdp = new CommonDataProvider(this);
			cdp.SetStringData("Code",Code);
			DataTable dt = DB.GetDataTable("SELECT * FROM EndOfDay where Symbol='"+Code+"' ORDER BY Date");
			double[] OPEN = new double[dt.Rows.Count];
			double[] HIGH =  new double[dt.Rows.Count];
			double[] LOW =  new double[dt.Rows.Count];
			double[] CLOSE =  new double[dt.Rows.Count];
			double[] VOLUME =  new double[dt.Rows.Count];
			double[] AMOUNT =  new double[dt.Rows.Count];
			double[] DATE = new double[dt.Rows.Count];

			for(int i=0; i<dt.Rows.Count; i++)
			{
				OPEN[i] = ToDouble(dt.Rows[i]["Open"]);
				HIGH[i] = ToDouble(dt.Rows[i]["High"]);
				LOW[i] = ToDouble(dt.Rows[i]["Low"]);
				CLOSE[i] = ToDouble(dt.Rows[i]["Close"]);
				VOLUME[i] = ToInt(dt.Rows[i]["Volume"]);
				DATE[i] = ((DateTime)dt.Rows[i]["Date"]).ToOADate();
			}
			cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
			return cdp;
		}
	}
}