using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Net;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Text;
using System.Globalization;
using System.Threading;
using System.IO;
using EasyTools;
using Easychart.Finance.DataClient;
using Easychart.Finance.DataProvider;

namespace WebDemos
{
	/// <summary>
	/// Summary description for DataService.
	/// </summary>
	public partial class DataService : System.Web.UI.Page
	{
		private string Exchange;

		static private Thread UpdateQuoteThread;
		static private DateTime UpdateQuoteDate;
		static private string UpdateQuoteExchange;
		static private int TotalCount = 0;
		static private int NowIndex = 0;
		static private int FixedCount = 0;

		static private int FixErrorCount;
	
		private void Bind()
		{
			btnUpdateWeb.Text = UpdateQuoteThread==null?"Update":"Stop";
			btnFixWrongData.Text = FixDataThread==null?"Fix wrong data":"Stop fix wrong data";

			DataTable dt = new DataTable();
			dt.Columns.Add("ConditionId",typeof(int));
			dt.Columns.Add("Description");
			dt.Columns.Add("Count",typeof(int));
			DataTable dtFixWrong = DB.GetDataTable("select ConditionId,count(*) from ScanedQuote where ConditionId<0 group by ConditionId");
			dt.Rows.Add(new object[]{-1,"Stocks should be deleted",0});
			dt.Rows.Add(new object[]{-2,"Network error, Should redownload",0});
			dt.Rows.Add(new object[]{-3,"No need to fix again",0});
			dt.Rows.Add(new object[]{-4,"Fixed symbols",0});
			foreach(DataRow dr1 in dt.Rows) 
			{
				foreach(DataRow dr2 in dtFixWrong.Rows)
					if (dr1[0].ToString()==dr2[0].ToString())  
					{
						dr1[2] = dr2[1];
						break;
					}
			}
			dgWrongList.DataSource = dt;
			dgWrongList.DataBind();

			if (FixDataThread!=null)
				lFixProgress.Text = "Total:"+TotalCount+";Progress:"+NowIndex+";Fixed:"+FixedCount+";Error:"+FixErrorCount;
			else lFixProgress.Text = "Stopped";
		}

		private void Page_Load(object sender, System.EventArgs e)
		{
			if (!IsPostBack)  
			{
				tbDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
				Bind();
			}
			Exchange = ddlExchange.SelectedItem.Value;
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();

			DataClientBase[] dcbs = DataClientBase.GetAllDataFeeds();
			foreach(DataClientBase dcb in dcbs)
				ddlDataFeed.Items.Add(dcb.ToString());

			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.btnImport.Click += new System.EventHandler(this.btnGet_Click);
			this.btnUpdateWeb.Click += new System.EventHandler(this.btnUpdateWeb_Click);
			this.btnDeleteExchange.Click += new System.EventHandler(this.btnDeleteExchange_Click);
			this.btnFixWrongData.Click += new System.EventHandler(this.btnFixWrongData_Click);
			this.dgWrongList.ItemCommand += new System.Web.UI.WebControls.DataGridCommandEventHandler(this.dgWrongList_ItemCommand);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		public static string[] DownloadData(string URL) 
		{
			byte[] bs = Utils.DownloadData(URL);
			string s = Encoding.ASCII.GetString(bs).Trim();
			if (s=="" || s==null)
				return new string[]{};
			return s.Split('\r');
		}

		private void btnGet_Click(object sender, System.EventArgs e)
		{
			string[] ss = DownloadData("http://data.easychart.net/Symbol.aspx?Exchange="+Exchange);

			DbParam[] dps = new DbParam[]{
											 new DbParam("@QuoteCode",DbType.String,""),
											 new DbParam("@QuoteName",DbType.String,""),
											 new DbParam("@Exchange",DbType.String,""),
			};
			int succ=0;
			int failed=0;
			BaseDb bd = DB.Open(false);
			try
			{
				for(int i=0; i<ss.Length; i++) 
				{
					string[] rr = ss[i].Trim().Split(';');
					if (rr.Length!=3) continue;
					dps[0].Value = rr[0].Trim();
					dps[1].Value = rr[1].Trim();
					dps[2].Value = rr[2].Trim();
					try 
					{
						if (bd.GetFirstRow("select QuoteCode from StockData where QuoteCode=?",new DbParam[]{dps[0]})==null)
						{
							bd.DoCommand("insert into stockdata (QuoteCode,QuoteName,Exchange) values (?,?,?)",dps);
							succ++;
						}
					} 
					catch
					{
						failed++;
					}
				}
			} 
			finally 
			{
				bd.Close();
			}
			lExchangeMsg.Text ="succ: "+succ+"; failed "+failed;
		}

		public static bool UpdateQuote(DateTime d,string Exchange) 
		{
			try 
			{
				string s = d.ToString("yyyy-MM-dd",DateTimeFormatInfo.InvariantInfo);
				string[] ss = DownloadData("http://data.easychart.net/eod.aspx?exchange="+Exchange+"&date="+s);
				if (ss.Length>0)
					InternetDataToDB.MergeOneDay(ss);
				Tools.Log("Update Quote:"+Exchange+":"+ss.Length+":"+d);
				Admin.UpdatePreScan.ClearCache();
				return ss.Length>0;
			} 
			catch (Exception e)
			{
				Tools.Log("Update Quote:"+e.Message);
				throw;
			}
		}

		static private void DoUpdateQuote()
		{
			Impersonate.ChangeToAdmin();
			try
			{
				UpdateQuote(UpdateQuoteDate,UpdateQuoteExchange);
			}			
			finally
			{
				UpdateQuoteThread = null;
			}
		}

		private void btnUpdateWeb_Click(object sender, System.EventArgs e)
		{
			if (UpdateQuoteThread==null)
			{
				UpdateQuoteDate = DateTime.Parse(tbDate.Text);
				UpdateQuoteExchange = ddlExchange.SelectedItem.Value;
				UpdateQuoteThread = new Thread(new ThreadStart(DoUpdateQuote));
				UpdateQuoteThread.Start();
			} 
			else 
			{
				UpdateQuoteThread.Abort();
				UpdateQuoteThread.Join();
			}
			Bind();

			//UpdateQuote(DateTime.Parse(tbDate.Text),ddlExchange.SelectedItem.Value);
		}

		private void btnDeleteExchange_Click(object sender, System.EventArgs e)
		{
			lExchangeMsg.Text = DB.DoCommand("delete from StockData where "+Utils.GetExchangeCond(Exchange))+" deleted";
		}


		public enum FixDataResult {OK,NoData,Others};
		static private FixDataResult TestGood(IDataManager idm,string Symbol,bool OnlyUp2Date)
		{
			IDataProvider idp = idm[Symbol,Config.FixDataTestDays];
			double[] High = idp["HIGH"];
			double[] Low = idp["LOW"];
			double[] Date = idp["DATE"];
			FixDataResult fdr = FixDataResult.OK;
			for(int i=Date.Length-1; i>=1; i--)
			{
				if (!OnlyUp2Date)
				{
					double d1 = Low[i]/High[i-1];//dd[i]/dd[i-1];
					double d2 = High[i]/Low[i-1];
					double Delta = Config.FixDataDifference/100.0;
					if (d2<(1.0-Delta) || d1>(1.0+Delta)) 
					{
						Tools.Log(Symbol+":Delta = "+d1+","+d2+":"+DateTime.FromOADate(Date[i]));
						fdr = FixDataResult.Others;
					}

					if (Date[i]-Date[i-1]>Config.FixDataGapDays) 
					{
						Tools.Log(Symbol+":GapDays:"+DateTime.FromOADate(Date[i]));
						fdr = FixDataResult.Others;
					}
				}
				if (i==Date.Length-1)
					if ((DateTime.Now.ToOADate()-Date[i])>Config.FixDataNoDataDays) 
					{
						Tools.Log(Symbol+":NoData:"+DateTime.FromOADate(Date[i]));
						fdr = FixDataResult.NoData;
					}
			}
			return fdr; 
		}

		static private void InsertQuote(int Id,string Symbol)
		{
			try
			{
				DB.DoCommand("insert into ScanedQuote (ConditionId,QuoteCode) values ("+Id+",'"+Symbol+"')");
			}
			catch
			{
			}

		}

		/// <summary>
		/// Test days : FixData.TestDays
		/// When data increase or decline beyond +/-"FixData.Difference"% , reload data
		/// When data lost for "FixData.GapDays" days, reload data, still no data then delete
		/// When no data for latest "FixData.NoDataDays" days, reload data, still no data then delete
		/// 
		/// Result saved in QuoteCode
		/// -1 : Stocks should be deleted
		/// -2 : Network error, Should redownload
		/// -3 : No need to fix again
		/// -4 : Fixed symbols
		/// </summary>
		/// <param name="Sender"></param>
		static public void FixData(object Sender)
		{
			Tools.Log("FixData started");
			try
			{
				DataTable dtSkip = DB.GetDataTable("select QuoteCode from ScanedQuote where ConditionId=-3");
				ArrayList alSkip = new ArrayList();
				foreach(DataRow dr in dtSkip.Rows)
					alSkip.Add(dr[0].ToString());

				DB.DoCommand("delete from ScanedQuote where ConditionId<0 and ConditionId<>-3");
				DataTable dt = DB.GetDataTable("select QuoteCode from stockdata");
				DBDataManager ddm = new DBDataManager();
				FixedCount = 0;
				FixErrorCount = 0;
				TotalCount = dt.Rows.Count;
				for(int j=0; j<dt.Rows.Count; j++)
				{
					NowIndex = j+1;
					DataRow dr = dt.Rows[j];
					string Symbol = dr["QuoteCode"].ToString();
					try
					{
						if (TestGood(ddm,Symbol,false)!=FixDataResult.OK)
						{
							if (alSkip.IndexOf(Symbol)>=0) 
							{
								if (TestGood(ddm,Symbol,true)==FixDataResult.OK) 
									continue;
								else 
									DB.DoCommand("delete from ScanedQuote where ConditionId=-3 and QuoteCode='"+Symbol+"'");
							}
							
							for(int k=0; k<3; k++)
							try
							{
								Utils.DownloadYahooHistory(Symbol,true,false);
								break;
							} 
							catch (WebException)
							{
								if (k==2) throw;
							}

							FixDataResult fdr1 = TestGood(ddm,Symbol,false);
							FixDataResult fdr2 = TestGood(ddm,Symbol,true);
							if (fdr2==FixDataResult.NoData)
								InsertQuote(-1,Symbol);
							else if (fdr1==FixDataResult.Others && fdr2==FixDataResult.OK)
								InsertQuote(-3,Symbol);
							else InsertQuote(-4,Symbol);
							FixedCount++;
						}
					}
					catch (Exception e)
					{
						if (e.Message.IndexOf("404")>=0)
							InsertQuote(-1,Symbol);
						else InsertQuote(-2,Symbol);
						Tools.Log("FixData:"+Symbol+";"+e.Message+";"+e.GetType());
						FixErrorCount++;
					}
					Thread.Sleep(1);
				}
				Tools.Log("FixData finished");
			} 
			finally
			{
				FixDataThread = null;
			}
		}

		public static void FixData()
		{
			FixData(null);
		}

		public static void DeleteWrongData(object sender)
		{
			BaseDb bd = DB.Open(true);
			try
			{
				bd.DoCommand("delete from StockData where QuoteCode in (select QuoteCode from ScanedQuote where ConditionId=-1)");
				bd.DoCommand("delete from Realtime where QuoteCode in (select QuoteCode from ScanedQuote where ConditionId=-1)");
				bd.DoCommand("delete from ScanedQuote where ConditionId=-1");
			}
			finally
			{
				bd.Close();
			}
		}

		public static void DeleteWrongData()
		{
			DeleteWrongData(null);
		}

		static private Thread FixDataThread;
		private void btnFixWrongData_Click(object sender, System.EventArgs e)
		{
			if (FixDataThread==null)
			{
				FixDataThread = new Thread(new ThreadStart(FixData));
				FixDataThread.Start();
			} 
			else 
			{
				FixDataThread.Abort();
				FixDataThread.Join();
				FixDataThread = null;
			}
			Bind();
		}

		public static void ClearTimeOfHistoricalData(object Sender)
		{
			DataTable dt = DB.GetDataTable("select QuoteCode from stockdata");
			DBDataManager ddm = new DBDataManager();
			for(int j=0; j<dt.Rows.Count; j++) 
			{
				DataRow dr = dt.Rows[j];
				string Code = dr["QuoteCode"].ToString();
				try
				{
					CommonDataProvider cdp = (CommonDataProvider)ddm[Code];
					cdp.TrimTime();
					cdp.SaveBinary(DBDataManager.GetHisDataFile(Code));
					Thread.Sleep(1);
					if ((j % 100)==0)
						Tools.Log("Clear time of historical data progress:"+j+"/"+dt.Rows.Count);
				}
				catch (Exception e)
				{
					Tools.Log("Clear time of histoical data:"+Code+";"+e);
					throw;
				}
			}
		}

		private void dgWrongList_ItemCommand(object source, System.Web.UI.WebControls.DataGridCommandEventArgs e)
		{
			if (e.CommandName=="Delete") 
			{
				int ConditionId = (int)dgWrongList.DataKeys[e.Item.ItemIndex];
				if (ConditionId==-1)
					DeleteWrongData();
			}
			Bind();
		}
	}
}
