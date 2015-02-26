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
using System.IO;
using System.Text;
using System.Globalization;
using Easychart.Finance.DataProvider;
using WebDemos.DBDemos;

namespace WebDemos.Admin
{
	/// <summary>
	/// Summary description for AddRemoveData.
	/// </summary>
	public partial class AddRemoveData : System.Web.UI.Page
	{

		protected MyDatePicker dpStart;
		
		protected MyDatePicker dpEnd;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			dpEnd.MaxYear = 2030;
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.btnMerge.Click += new System.EventHandler(this.btnMerge_Click);
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
			this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void GetFormatInfo(ref int DateIndex,ref int TickerIndex,out string[] ssHeader,out string DateFormat)
		{
			ssHeader = ddlFormat.SelectedValue.Split(',');
			DateIndex = 0;
			TickerIndex = -1;
			DateFormat = ddlDateFormat.SelectedItem.Value;
			for(int i=0; i<ssHeader.Length; i++)
			{
				if (ssHeader[i].StartsWith("DATE("))
				{
					DateIndex = i;
					if (DateFormat=="")
						DateFormat = ssHeader[i].Substring(5,ssHeader[i].Length-6);
				} 
				else if (ssHeader[i]=="TICKER")
					TickerIndex = i;
			}
		}

		private char GetSeperator()
		{
			string rr = ddlSeparator.SelectedValue;
			char r = rr[0];
			if (rr=="Tab") 
				r = '\t';
			return r;
		}

		private void btnMerge_Click(object sender, System.EventArgs e)
		{
			string[] Keys = {"OPEN","HIGH","LOW","CLOSE","VOLUME","DATE","ADJCLOSE"};
			string[] ss = tbCSVData.Text.Trim().Split('\n');
			
			int DateIndex = 0;
			int TickerIndex = -1;
			string[] ssHeader;
			string DateFormat;
			GetFormatInfo(ref DateIndex,ref TickerIndex,out ssHeader,out DateFormat);
	
			SortedList slAllSymbol = new SortedList(Comparer.Default);
			SortedList slOneSymbol;

			char r = GetSeperator();

			for(int i=cbHasHeader.Checked?1:0; i<ss.Length; i++)
			{
				string[] sss = ss[i].Trim().Split(r);
				try 
				{
					string Ticker = tbSymbol.Text;
					if (TickerIndex>=0)
						Ticker = sss[TickerIndex];
					slOneSymbol = (SortedList)slAllSymbol[Ticker];
					if (Ticker=="")
						throw new Exception("Symbol can't be empty!");
					if (slOneSymbol==null)
					{
						slOneSymbol = new SortedList(Comparer.Default);
						slAllSymbol[Ticker] = slOneSymbol;
					}

					slOneSymbol[DateTime.ParseExact(sss[DateIndex].Trim(),
						DateFormat,
						DateTimeFormatInfo.InvariantInfo)
						] = sss;
				}
				catch (Exception ex)
				{
					throw new Exception(ex.Message+";"+sss[DateIndex]+";"+DateFormat);
				}
			}

			lMsg.Text = "";
			DBDataManager ddm = new DBDataManager();
			try 
			{
				foreach(string s in slAllSymbol.Keys) 
				{
					slOneSymbol = (SortedList)slAllSymbol[s];
					double[][] ds = new double[7][];
					for(int i=0; i<ds.Length; i++)
					{
						ds[i] = new double[slOneSymbol.Count];
						for(int j=0; j<ds[i].Length; j++)
							ds[i][j] = double.NaN;
					}

					for(int i=0; i<slOneSymbol.Count; i++)
					{
						ds[5][i] = ((DateTime)slOneSymbol.GetKey(i)).ToOADate();
						for(int j=0; j<ssHeader.Length; j++) 
							if (j!=DateIndex && j!=TickerIndex)
							{
								int k = Array.IndexOf(Keys,ssHeader[j].ToUpper());
								if (k>=0) 
								{
									string[] sss = (string[])slOneSymbol.GetByIndex(i);
									ds[k][i] = double.Parse(sss[j]);
								}
							}
						for(int j=0; j<ds.Length; j++)
							if (double.IsNaN(ds[j][i]))
								ds[j][i] = ds[3][i];
					}

					CommonDataProvider cdp = (CommonDataProvider)ddm[s];
					CommonDataProvider cdpDelta = new CommonDataProvider(null);
					cdpDelta.LoadBinary(ds);

					lMsg.Text += "Symbol:"+s+"; Original data count :"+cdp.Count+"; Merge data count : "+cdpDelta.Count+"; ";
					cdp.Merge(cdpDelta);
					lMsg.Text +="New data count : "+cdp.Count+"<br>";
					Impersonate.ChangeToAdmin();
					Utils.UpdateRealtime(s,cdp);
					cdp.SaveBinary(DBDataManager.GetHisDataFile(s));
				}
			}
			catch (Exception ex)
			{
				lMsg.Text = ex.Message;
			}
		}

		private void btnDelete_Click(object sender, System.EventArgs e)
		{
			DBDataManager ddm = new DBDataManager();
			try 
			{
				CommonDataProvider cdp = (CommonDataProvider)ddm[tbSymbol.Text];
				lMsg.Text = "Original data count :"+cdp.Count+"<br>";
				cdp.DeleteData(dpStart.Date,dpEnd.Date);
				lMsg.Text +="New data count : "+cdp.Count;
				string Filename = DBDataManager.GetHisDataFile(tbSymbol.Text);
				if (File.Exists(Filename))
					File.Delete(Filename);
				Utils.UpdateRealtime(tbSymbol.Text,cdp);
				cdp.SaveBinary(Filename);
			} 
			catch (Exception ex)
			{
				lMsg.Text = ex.Message;
			}
		}

		private void btnClear_Click(object sender, System.EventArgs e)
		{
			try 
			{
				string Filename = DBDataManager.GetHisDataFile(tbSymbol.Text);
				File.Delete(Filename);
			} 
			catch (Exception ex)
			{
				lMsg.Text = ex.Message;
			}
		}

		private void btnExport_Click(object sender, System.EventArgs e)
		{
			DBDataManager ddm = new DBDataManager();
			try 
			{
				int DateIndex = 0;
				int TickerIndex = -1;
				string[] ssHeader;
				string DateFormat;
				GetFormatInfo(ref DateIndex,ref TickerIndex,out ssHeader,out DateFormat);

				CommonDataProvider cdp = (CommonDataProvider)ddm[tbSymbol.Text];
				StringBuilder sb = new StringBuilder();
				if (cbHasHeader.Checked)
					sb.Append(ddlFormat.SelectedItem.Text+"\r\n");
				char r = GetSeperator();
				for(int i=0; i<cdp.Count; i++)
				{
					for(int j=0; j<ssHeader.Length; j++) 
					{
						if (j==DateIndex) 
						{
							sb.Append(DateTime.FromOADate(cdp["DATE"][i]).ToString(DateFormat,DateTimeFormatInfo.InvariantInfo));
						} 
						else if (j==TickerIndex)
						{
							sb.Append(cdp.GetStringData("Code"));
						}
						else 
						{
							try
							{
								double[] dd = cdp[ssHeader[j]];
								if (dd!=null)
									sb.Append(dd[i].ToString("f2"));
							}
							catch
							{
							}
						}
						sb.Append(r);
					}
					sb.Remove(sb.Length-1,1);
					sb.Append("\r\n");
				}
				tbCSVData.Text = sb.ToString();
			} 
			catch (Exception ex)
			{
				lMsg.Text = ex.Message;
			}
		}
	}
}