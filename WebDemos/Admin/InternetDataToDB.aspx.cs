using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Net;
using System.IO;
using System.Text;
using System.Globalization;
using EasyTools;
using System.Data.SqlClient;
using Easychart.Finance.DataProvider;
using System.Threading;
using WebDemos;

namespace WebDemos
{
	public partial class InternetDataToDB : System.Web.UI.Page
	{

		string Exchange;

		static int DownloadMode;
		static string DownloadProgress;

		Thread DownloadHistoryThread
		{
			get 
			{
				return (Thread)Application["HistoryThread"];
			}
			set 
			{
				Application["HistoryThread"] = value;
			}
		}

		bool StopDownload 
		{
			get 
			{
				return (bool)Application["StopDownload"];
			}
			set 
			{
				Application["StopDownload"] = value;
			}
		}

		private void Bind() 
		{
			if (DownloadHistoryThread==null)
				btnDownload.Text = "Start";
			else 
				btnDownload.Text = "Stop";
			if (lProgress!=null)
				lProgress.Text = DownloadProgress;
			if (rblMode!=null)
				rblMode.SelectedIndex = DownloadMode;
		}
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			if (!IsPostBack) 
			{
				tbDate.Text = DateTime.Now.ToString("yyyy-MM-dd");
				if (tbDateBefore!=null)
					tbDateBefore.Text = DateTime.Now.ToString("yyyy-MM-dd");
				Bind();
			}
			Exchange = ddlExchange.SelectedItem.Value;
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			InitializeComponent();
			base.OnInit(e);
		}
		
		private void InitializeComponent()
		{    
			this.btnGet.Click += new System.EventHandler(this.btnGet_Click);
			this.btnRemain.Click += new System.EventHandler(this.btnRemain_Click);
			this.btnDelete.Click += new System.EventHandler(this.btnRemain_Click);
			this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
			this.btnDownload.Click += new System.EventHandler(this.btnDownload_Click);
			this.btnUpdateWeb.Click += new System.EventHandler(this.btnUpdateWeb_Click);
			this.btnTestFile.Click += new System.EventHandler(this.btnTestFile_Click);
			this.btnCreateFile.Click += new System.EventHandler(this.btnCreateFile_Click);
			this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
			this.btnUpdateRealtime.Click += new System.EventHandler(this.btnUpdateRealtime_Click);
			this.btnClearCache.Click += new System.EventHandler(this.btnClearCache_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		public static string[] DownloadData(string URL) 
		{
			HttpWebRequest hwr = (HttpWebRequest)WebRequest.Create(URL);
			hwr.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705; .NET CLR 1.1.4322)";
			hwr.Accept = "*/*";
			hwr.KeepAlive = false;
			hwr.Referer = "http://www.eoddata.com";
			hwr.Headers["Accept-Encoding"] = "gzip, deflate";
			hwr.Headers["Accept-Language"] = "zh-cn";
			hwr.Headers["Pragma"] = "no-cache";
			hwr.Headers["Cache-Control"] = "no-cache";
			hwr.CookieContainer = new CookieContainer();
			hwr.CookieContainer.Add(new Cookie("chartdata","e=support%40easychart%2Enet","/","www.eoddata.com"));
			hwr.CookieContainer.Add(new Cookie("chartdata","e=support%40easychart%2Enet","/","eoddata.com"));

			HttpWebResponse hws= (HttpWebResponse)hwr.GetResponse();
			StreamReader sr = new StreamReader(hws.GetResponseStream(),Encoding.ASCII);
			ArrayList al = new ArrayList();
			while (true) 
			{
				string r  = sr.ReadLine();
				if (r==null) break;
				al.Add(r);
			}
			sr.Close();
			hws.Close();
			return (string[])al.ToArray(typeof(string));
		}

		public static void MergeOneDay(string[] ss) 
		{
			DbParam[] dps = new DbParam[]{
											 new DbParam("@QuoteCode",DbType.String,null),
			};

			for (int i=0; i<ss.Length; i++)
				if (ss[i]!="")
					try
					{
						DataPacket dp = DataPacket.ParseEODData(ss[i]);
						dps[0].Value = dp.Symbol;
						DataRow dr= DB.GetFirstRow("select * from StockData where QuoteCode=?",dps);
						if (dr!=null) 
						{
							string QuoteCode = dr["QuoteCode"].ToString();
							byte[] bs = DBDataManager.LoadHisDataFromFile(QuoteCode);
							bs = CommonDataProvider.MergeOneQuote(bs,dp);
							CommonDataProvider cdp = new CommonDataProvider(null);
							cdp.LoadBinary(bs);
							Utils.UpdateRealtime(QuoteCode,cdp);
							DBDataManager.SaveBinary(QuoteCode,bs);
						}
					} 
					catch (Exception e)
					{
						Tools.Log("Merge Data:"+e.Message);
					}
		}

		public static bool UpdateQuote(DateTime d,string Exchange) 
		{
			try 
			{
				Impersonate.ChangeToAdmin();
				string s = d.ToString("yyyyMMMdd",DateTimeFormatInfo.InvariantInfo);
				string[] ss = DownloadData("http://www.eoddata.com/Data.asp?e="+Exchange+"&d="+s);
				if (string.Compare(Exchange,"INDEX",true)==0)
					for(int i=0; i<ss.Length; i++)
						ss[i] = "^"+ss[i];
				if (ss.Length>0)
					MergeOneDay(ss);
				return ss.Length>0;
			} 
			catch (Exception e)
			{
				Tools.Log("Update Quote:"+e.Message);
				throw;
			}
		}

		private void btnDownload_Click(object sender, System.EventArgs e)
		{
			if (DownloadHistoryThread==null) 
			{
				DownloadMode = 0;
				if (rblMode!=null)
					DownloadMode = rblMode.SelectedIndex;
				StopDownload = false;
				DownloadHistoryThread = new Thread(new ThreadStart(DownloadHistory));
				DownloadHistoryThread.Start();
			} 
			else 
			{
				StopDownload = true;
				DownloadHistoryThread = null;
			}
			Bind();
		}

		private void btnGet_Click(object sender, System.EventArgs e)
		{
			string[] ss = DownloadData("http://eoddata.com/SymbolList.asp?e="+Exchange);

			DbParam[] dps = new DbParam[]{
											 new DbParam("@QuoteCode",DbType.String,""),
											 new DbParam("@QuoteName",DbType.String,""),
											 new DbParam("@Exchange",DbType.String,Exchange),
			};
			int succ=0;
			int failed=0;
			BaseDb bd = DB.Open(false);
			try
			{
				for(int i=1; i<ss.Length; i++) 
				{
					string[] rr = ss[i].Split('\t');
					if (rr.Length!=2) continue;
					if (Exchange=="INDEX")
						dps[0].Value = "^"+rr[0].Trim();
					else dps[0].Value = rr[0].Trim();
					dps[1].Value = rr[1].Trim();
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

		private void btnUpdate_Click(object sender, System.EventArgs e)
		{
		}

		public void DownloadHistory() 
		{
			Impersonate.ChangeToAdmin();
			try
			{
				DB.DoCommand("update StockData set HasHistory=0");
				string Where = "";
				if (DownloadMode==1)
					Where = " where LastTime<'"+tbDateBefore.Text+"' or LastTime is null ";
				DataTable dt = DB.GetDataTable(
					"select a.QuoteCode,a.QuoteName,a.HasHistory,b.LastTime from StockData a "+
					" left join realtime b on a.QuoteCode=b.QuoteCode "+Where+" order by a.QuoteCode");
			
				DbParam[] dps = new DbParam[]{
												 new DbParam("@QuoteCode",DbType.String,null),
				};
				DownloadProgress = "Download started:"+dt.Rows.Count;
				for(int i=0; i<dt.Rows.Count; i++)
					try
					{
						DataRow dr = dt.Rows[i];
						if (StopDownload) break;
						string QuoteCode = dr["QuoteCode"].ToString();
						Utils.DownloadYahooHistory(QuoteCode,DownloadMode!=0,false);
						Tools.Log("Downloaded "+QuoteCode+" from yahoo finance");
						dps[0].Value = QuoteCode;
						DB.DoCommand("update StockData set HasHistory=1 where QuoteCode=?",dps);
						DownloadProgress = (i+1)+"/"+dt.Rows.Count;
					}
					catch (Exception ex)
					{
						Tools.Log(ex.Message);
					}
			} 
			finally
			{
				DownloadProgress = "Download history finished!";
				DownloadHistoryThread = null;
			}
		}

		private void btnUpdateWeb_Click(object sender, System.EventArgs e)
		{
			UpdateQuote(DateTime.Parse(tbDate.Text),ddlExchange.SelectedItem.Value);
		}

		private void btnRemain_Click(object sender, System.EventArgs e)
		{
			string[] ss = tbRemain.Text.Split('\r');
			for(int i=0; i<ss.Length; i++) 
			{
				string[] sss = ss[i].Split(';');
				if (sss.Length>0)
					ss[i] = sss[0].Trim();
				else ss[i] = ss[i].Trim();
			}
			string r = "";
			if (sender == btnRemain)
				r = " not ";
			DB.DoCommand("delete from StockData where QuoteCode "+r+" in ('"+string.Join("','",ss)+"')");
			ClearCache();
		}

		private void btnTestFile_Click(object sender, System.EventArgs e)
		{
			string Root = HttpRuntime.AppDomainAppPath+"Data\\";
			Random Rnd = new Random();
			for(int i=0; i<4000; i++) 
			{
				FileStream fs = File.OpenRead(Root+i/*Rnd.Next(4000)*/+".dat");
				byte[] bs = new byte[56];
				fs.Seek(-bs.Length,SeekOrigin.End);
				fs.Read(bs,0,bs.Length);
				fs.Close();
			}
		}

		private void btnCreateFile_Click(object sender, System.EventArgs e)
		{
			string Root = HttpRuntime.AppDomainAppPath+"Data\\";
			for(int i=0; i<4000; i++) 
			{
				FileStream fs = File.OpenWrite(Root+i+".dat");
				byte[] bs = new byte[28*5000];
				fs.Write(bs,0,bs.Length);
				fs.Close();
			}
		}

		private void btnAdd_Click(object sender, System.EventArgs e)
		{
			string[] ss = tbAddSymbol.Text.Split('\n');
			DbParam[] dps = new DbParam[]{
											new DbParam("@QuoteCode",DbType.String,""),
											new DbParam("@QuoteName",DbType.String,""),
											new DbParam("@Exchange",DbType.String,"")
										 };
			int succ=0; 
			int failed=0;
			foreach(string s in ss)
				if (s!="")
				{
					string[] sss = s.Trim().Split(';');
					try 
					{
						for(int i=0; i<sss.Length; i++)
							dps[i].Value = sss[i];
						DB.DoCommand("insert into StockData (QuoteCode,QuoteName,Exchange) values (?,?,?)",dps);
						succ++;
					} 
					catch 
					{
						failed++;
					}
				}
			lAddMsg.Text = "succ:"+succ+" failed:"+failed;
			ClearCache();
		}

		private void btnExport_Click(object sender, System.EventArgs e)
		{
			DataTable dt = DB.GetDataTable("select QuoteCode,QuoteName,Exchange from stockdata order by QuoteCode");
			StringBuilder sb = new StringBuilder();
			foreach(DataRow dr in dt.Rows) 
				sb.Append(dr[0]+";"+dr[1]+";"+dr[2]+"\r");
			tbExport.Text = sb.ToString();
		}

		private void ClearCache() 
		{
			Cache.Remove("QuoteList");
		}

		private void btnClearCache_Click(object sender, System.EventArgs e)
		{
			ClearCache();
		}

		public static void UpdateRealtime(object Sender)
		{
			DataTable dt = DB.GetDataTable("select QuoteCode from stockdata");
			DBDataManager ddm = new DBDataManager();
			foreach(DataRow dr in dt.Rows)
			{
				string Code = dr["QuoteCode"].ToString();
				IDataProvider idp = ddm[Code,2];
				Utils.UpdateRealtime(Code,idp);
				Thread.Sleep(1);
			}
		}

		private void btnUpdateRealtime_Click(object sender, System.EventArgs e)
		{
			UpdateRealtime(this);
		}
	}
}
