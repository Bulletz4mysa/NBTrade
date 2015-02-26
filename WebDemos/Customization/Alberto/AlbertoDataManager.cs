using System;
using System.Data;
using Easychart.Finance.DataProvider;
using System.Web.UI.WebControls;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// Summary description for AlbertoDataManager.
	/// </summary>
	public class AlbertoDataManager:DataManagerBase
	{
		public AlbertoDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			CommonDataProvider cdp = new CommonDataProvider(this);
			cdp.SetStringData("Code",Code);
			DataTable dt = DB.GetDataTable("SELECT top "+Count+" * FROM WEBCHARTX_DATA where Symbol='"+Code+"' ORDER BY Store_Date");
			double[] OPEN = new double[dt.Rows.Count];
			double[] HIGH =  new double[dt.Rows.Count];
			double[] LOW =  new double[dt.Rows.Count];
			double[] CLOSE =  new double[dt.Rows.Count];
			double[] VOLUME =  new double[dt.Rows.Count];
			double[] AMOUNT =  new double[dt.Rows.Count];
			double[] DATE = new double[dt.Rows.Count];

			for(int i=0; i<dt.Rows.Count; i++)
			{
				OPEN[i] = ToDecimalDouble(dt.Rows[i]["Open_Price"]);
				HIGH[i] = ToDecimalDouble(dt.Rows[i]["HIGHT_PRICE"]);
				LOW[i] = ToDecimalDouble(dt.Rows[i]["LOW_PRICE"]);
				CLOSE[i] = ToDecimalDouble(dt.Rows[i]["CLOSE_PRICE"]);
				VOLUME[i] = ToInt(dt.Rows[i]["VOLUME"]);
				DATE[i] = ((DateTime)dt.Rows[i]["STORE_DATE"]).ToOADate();
			}
			cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
			return cdp;
		}

		public override DataTable Exchanges
		{
			get
			{
				DataTable dt = DB.GetDataTable("select distinct Exchange as Value from SYMBOLS where enabled = 1");
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
			string Where = " where enabled = 1";
			if (ConditionId!=null) 
				Where = ",ScanedQuote b where a.Symbol=b.QuoteCode and b.ConditionId in ("+ConditionId+")";
			if (Exchange!=null && Exchange!="")
				Where +=" and a.Exchange='"+Exchange+"'"; 
			if (Key!=null) 
				Where +=" and (a.Symbol like '%"+Key+"%')";
			return Where;
		}

		public override DataTable GetStockList(string Exchange, string ConditionId, string Key, string Sort, int StartRecords, int MaxRecords)
		{
			if (Sort==null)
				Sort = "";
			if (Sort!="")
				Sort =" Order by "+Sort;
			DataTable dt =DB.GetDataTable("select a.Symbol,a.Name,a.Exchange from SYMBOLS a "
				+GetWhere(Exchange,ConditionId,Key)+Sort, null,StartRecords,MaxRecords); 
			return dt;
		}

		public override int SymbolCount(string Exchange, string ConditionId, string Key)
		{
			DataTable dt = DB.GetDataTable("select symbol from SYMBOLS a "+GetWhere(Exchange,ConditionId,Key));
			return dt.Rows.Count;
		}

		public override DataTable GetSymbolList(string Exchange,string ConditionId,string Key,string Sort,int StartRecords,int MaxRecords)
		{
			DataTable dt =DB.GetDataTable("select a.symbol from SYMBOLS a "+GetWhere(Exchange,ConditionId,Key) + " order by a.symbol",null,StartRecords,MaxRecords); 
			return dt;
		}

		public override System.Web.UI.WebControls.DataGridColumn[] StockListColumns
		{
			get
			{
				return new DataGridColumn[]{
											   CreateHyperLinkColumn("Chart","symbol","customchart.aspx?"+Config.SymbolParameterName+"={0}"),
											   CreateBoundColumn("Symbol","Symbol","a.Symbol",null),
											   CreateBoundColumn("NAME","NAME","a.NAME",null),
											   CreateBoundColumn("Exchange","Exchange","Exchange",null),
				};
			}
		}
	}
}