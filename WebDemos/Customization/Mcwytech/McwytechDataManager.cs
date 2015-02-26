using System;
using System.Data;
using Easychart.Finance.DataProvider;
using System.Web.UI.WebControls;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// Summary description for McwytechDataManager.
	/// </summary>
	public class McwytechDataManager:DataManagerBase
	{
		public McwytechDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			try
			{
				CommonDataProvider cdp = new CommonDataProvider(this);
				cdp.SetStringData("Code",Code);
				//	cdp.SetStringData("Name",dr["CompanyName"].ToString());
				//	cdp.SetStringData("Exchange",dr["Exchange"].ToString());

				DataTable dt = DB.GetDataTable("SELECT top "+Count+" * from "+Code+"_1da order by tstamp desc");
				double[] OPEN = new double[dt.Rows.Count];
				double[] HIGH =  new double[dt.Rows.Count];
				double[] LOW =  new double[dt.Rows.Count];
				double[] CLOSE =  new double[dt.Rows.Count];
				double[] VOLUME =  new double[dt.Rows.Count];
				double[] AMOUNT =  new double[dt.Rows.Count];
				double[] DATE = new double[dt.Rows.Count];
				double LastClose = 0;
				for(int i=0; i<dt.Rows.Count; i++)
				{
					DataRow dr = dt.Rows[dt.Rows.Count-1-i];
					HIGH[i] = ToSingle(dr["priceh"]);
					LOW[i] = ToSingle(dr["pricel"]);
					CLOSE[i] = ToSingle(dr["pricec"]);
					OPEN[i] = Math.Min(Math.Max(LOW[i],LastClose),HIGH[i]);
					VOLUME[i] = 0;
					DATE[i] = ((DateTime)dr["tstamp"]).ToOADate();
					LastClose = CLOSE[i];
				}
				cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
				return cdp;
			} 
			catch
			{
			}
			return base.GetData (Code, Count);
		}

		public override DataTable Exchanges
		{
			get
			{
				DataTable dt = DB.GetDataTable("select distinct Exchange as Value from z_tickconfig");
				dt.Columns.Add("Text");
				for(int i=0; i<dt.Rows.Count; i++)
					dt.Rows[i][1] = dt.Rows[i][0];

				DataRow dr = dt.NewRow();
				dr[0] = "";
				dr[1] = "ALL";
				dt.Rows.InsertAt(dr,0);
				return dt;
			}
		}

		private string GetWhere(string Exchange, string ConditionId, string Key)
		{
			string Where = " where 1=1";
			if (ConditionId!=null) 
				Where = ",ScanedQuote b where a.Symbol=b.QuoteCode and b.ConditionId in ("+ConditionId+")";
			if (Key!=null) 
				Where +=" and (a.Symbol like '%"+Key+"%' or a.Description like '%"+Key+"%')";
			if (Exchange!=null && Exchange!="")
				Where +=" and a.Exchange='"+Exchange+"'"; 
			Where += " and a.type='da'";
			return Where;
		}
		
		public override DataTable GetStockList(string Exchange, string ConditionId, string Key, string Sort, int StartRecords, int MaxRecords)
		{
			if (Sort==null)
				Sort = "";
			if (Sort!="")
				Sort =" Order by "+Sort;
			DataTable dt =DB.GetDataTable("select a.symbol,a.ticker,a.Description,a.Exchange,a.ltick from z_config a "
				+GetWhere(Exchange,ConditionId,Key)+Sort, null,StartRecords,MaxRecords); 
			return dt;
		}

		public override int SymbolCount(string Exchange, string ConditionId, string Key)
		{
			DataRow drCount = 
				DB.GetFirstRow("select count(*) from z_config a"+GetWhere(Exchange,ConditionId,Key));
			return Tools.ToIntDef(drCount[0].ToString(),0);
		}

		public override DataTable GetSymbolList(string Exchange,string ConditionId,string Key,string Sort,int StartRecords,int MaxRecords)
		{
			DataTable dt =DB.GetDataTable("select a.symbol from z_config a "+GetWhere(Exchange,ConditionId,Key)); 
			return dt;
		}
		
		public override System.Web.UI.WebControls.DataGridColumn[] StockListColumns
		{
			get
			{
				return new DataGridColumn[]{
					CreateHyperLinkColumn("Chart","symbol","dchart.aspx?"+Config.SymbolParameterName+"={0}"),
					CreateBoundColumn("Symbol","ticker","a.ticker",null),
					CreateBoundColumn("Name","Description","Description",null),
					CreateBoundColumn("Exchange","Exchange","Exchange",null),
					CreateBoundColumn("Date","ltick","ltick","{0:yyyy-MM-dd}"),
				};
			}
		}
	}
}