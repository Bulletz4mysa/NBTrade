using System;
using System.Data;
using Easychart.Finance.DataProvider;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// Summary description for McwytechIntraDataManager.
	/// </summary>
	public class McwytechIntraDataManager:IntraDataManagerBase
	{
		public McwytechIntraDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			try
			{
				CommonDataProvider cdp = new CommonDataProvider(this);
				DbParam[] dps = new DbParam[]{
												 new DbParam("@Q1",DbType.DateTime,StartTime),
												 new DbParam("@Q2",DbType.DateTime,EndTime),
				};
				BaseDb bd = DB.Open(false);
				try
				{
					DataTable dt = bd.GetDataTable("select Tick,tstamp from "+Code+"_tick where tstamp>=? and tstamp<=? order by tstamp",dps);

					dps[1].Value = ((DateTime)dps[0].Value).AddSeconds(-1);
					dps[0].Value = ((DateTime)dps[0].Value).AddDays(-30);
					DataRow drr = bd.GetFirstRow("select Tick from "+Code+"_tick where tstamp>=? and tstamp<=? order by tstamp desc",dps);

					if (dt.Rows.Count>0)
						cdp.SetStringData("LastTradeTime",((DateTime)dt.Rows[dt.Rows.Count-1]["tstamp"]).ToString());
					if (drr!=null)
						cdp.SetStringData("LastPrice",drr[0].ToString());

					if (dt.Rows.Count==0)
						dt.Rows.Add(new object[]{Single.NaN,StartTime});
					double[] CLOSE =  new double[dt.Rows.Count];
					double[] VOLUME =  new double[dt.Rows.Count];
					double[] DATE = new double[dt.Rows.Count];

					for(int i=0; i<dt.Rows.Count; i++)
					{
						CLOSE[i] = ToDouble(dt.Rows[i]["Tick"]);
						VOLUME[i] = 0;
						DATE[i] = ((DateTime)dt.Rows[i]["tstamp"]).ToOADate();
					}
					cdp.LoadBinary("CLOSE",CLOSE);
					cdp.LoadBinary("DATE",DATE);
					cdp.LoadBinary("VOLUME",VOLUME);
				}
				finally
				{
					bd.Close();
				}
				return cdp;
			}
			catch
			{
			}
			return base.GetData (Code, Count);
		}
	}
}