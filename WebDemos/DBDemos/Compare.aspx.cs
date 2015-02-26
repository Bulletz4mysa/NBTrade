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
using System.Drawing.Imaging;
using Easychart.Finance;
using Easychart.Finance.DataProvider;
using EasyTools;
using WebDemos;
using System.Globalization;
using System.Web.Caching;

namespace WebDemos.DBDemos
{
	/// <summary>
	/// Summary description for Compare.
	/// </summary>
	public  partial class Compare : System.Web.UI.Page
	{
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			if (!IsPostBack)
				BindChart();
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

		}
		#endregion

		//Random Rnd = new Random();
		private void BindChart() 
		{
			string Quote1 = tbQuote1.Text;
			string Quote2 = tbQuote2.Text;

			FormulaChart fc = new FormulaChart();

			// Compare two closing data
			fc.AddArea("CmpIndi("+tbQuote2.Text+",CLOSE)",2);
			// Compare two RSI(14) of two stocks
			fc.AddArea("CmpIndi("+tbQuote2.Text+",RSI(14))");
			fc.AddArea("SlowSTO(14,3,3)",tbQuote1.Text,1);
			fc.AddArea("SlowSTO(14,3,3)",tbQuote2.Text,1);
			
			// Set skin
			fc.SetSkin(FormulaSkin.RedWhite);

			DataManagerBase dmb = Utils.GetDataManager(Config.DefaultDataManager);
			//MemoryStream ms = new MemoryStream();
			try 
			{
				// Bind data
				fc.DataProvider = dmb[Quote1];
				
				// Add another AxisY to the main panel
				FormulaAxisY fay = fc[0].AddNewAxisY(AxisPos.Left,true,1);

				// Bind another stock to the second AxisY
				//FormulaData fd = fc[0][1];
				//fd.Transform = Transform.Normal;
				//fd.AxisYIndex = 1;

				fc.ShowCursorLabel = true;

				DataTable dt = new DataTable();
				dt.Columns.Add("Symbol");
				dt.Columns.Add("Name");
				dt.Columns.Add("Last");
				dt.Columns.Add("Date");
				dt.Columns.Add("Open");
				dt.Columns.Add("High");
				dt.Columns.Add("Low");
				dt.Columns.Add("Close");
				dt.Columns.Add("Volume");
				dt.Columns.Add("Change");

				tblChart.Visible = true;
				int Width=(int)ibChart.Width.Value;
				if (Width<=0) Width = 780;
				int Height = (int)ibChart.Height.Value;
				if (Height<=0) Height = 850;
				
				ibChart.ImageUrl = "~/ImageFromCache.aspx?CacheId="+
					fc.SaveToImageStream(Width,Height,ImageFormat.Png,
						Tools.ToIntDef(Request.Form[ibChart.ID+".x"],0),
						Tools.ToIntDef(Request.Form[ibChart.ID+".y"],0));

				AddDataToTable(dt,fc.DataProvider,fc.CursorPos);
				IDataProvider idp2 = dmb[Quote2];
				idp2.BaseDataProvider = fc.DataProvider;
				AddDataToTable(dt,idp2,fc.CursorPos);

				dgStockData.DataSource = dt;
				dgStockData.DataBind();
			}
			catch (Exception ex)
			{
				lMsg.Text = ex.Message;
			}
		}

		private void AddDataToTable(DataTable dt , IDataProvider idp,int Pos) 
		{
			if (Pos>=0 && Pos<idp.Count) 
			{
				DataRow dr = dt.NewRow();
				double Close = idp["CLOSE"][Pos];
				double Last = 0;
				if (Pos>0)
					Last = idp["CLOSE"][Pos-1];

				dr["Symbol"] = idp.GetStringData("Code");
				dr["Name"] = idp.GetStringData("Name");
				try 
				{
					dr["Date"] = DateTime.FromOADate(idp["DATE"][Pos]).ToString("dd-MMM-yyyy dddd",DateTimeFormatInfo.InvariantInfo);
				}
				catch
				{
				}
				dr["Open"] = idp["OPEN"][Pos].ToString("f2",NumberFormatInfo.InvariantInfo);
				dr["High"] = idp["HIGH"][Pos].ToString("f2",NumberFormatInfo.InvariantInfo);
				dr["Low"] = idp["LOW"][Pos].ToString("f2",NumberFormatInfo.InvariantInfo);
				dr["Close"] = Close.ToString("f2",NumberFormatInfo.InvariantInfo);
				dr["Last"] = Last.ToString("f2",NumberFormatInfo.InvariantInfo);
				dr["Volume"] = idp["VOLUME"][Pos].ToString("f0",NumberFormatInfo.InvariantInfo);
				dr["Change"] = ((Close-Last)/Last).ToString("p2",NumberFormatInfo.InvariantInfo);
				dt.Rows.Add(dr);
			}
		}

		protected void btnUpdate_Click(object sender, System.EventArgs e)
		{
			BindChart();
		}

		protected void ibChart_Click(object sender, System.Web.UI.ImageClickEventArgs e)
		{
			BindChart();
		}
	}
}
