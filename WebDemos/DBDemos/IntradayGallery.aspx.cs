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
using Easychart.Finance;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// Summary description for IntradayGallery.
	/// </summary>
	public  partial class IntradayGallery : System.Web.UI.Page
	{
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			int i = 0;
			int Cols = Tools.ToIntDef(Request.QueryString["Cols"],3);
			foreach(string s in Config.IntradaySymbols.Split(';')) 
			{
				string r = Utils.GetPart1(s);
				ExchangeIntraday ei = ExchangeIntraday.GetExchangeIntraday(Utils.GetExchange(s));
				DateTime D = ei.GetCurrentTradingDay();

				HyperLink hl= new HyperLink();
				hl.ImageUrl = "~/Chart.aspx?Provider="+Config.IntraDataManager+"&Code="+r+
					"&Start="+D.Date.ToString("yyyyMMddHHmmss")+"&End="+D.AddDays(1).Date.AddMinutes(-1).ToString("yyyyMMddHHmmss")+
					"&Main=MainArea&Type=1&Scale=0&Skin="+Config.IntradaySkin+"&size="+Config.IntradayGallerySize+
					"&Layout="+Config.LayoutForSmall+"&XCycle=Hour2&Cycle=Minute2"+ //&YFormat=f0
					"&SV=0&BMargin=20&Fix=1"; 
				if (Config.IntradayPopup)
					hl.Target = "_blank";
				
				string Navi = "Intraday.aspx?(Symbol)";
				if (lNavigate!=null)
					Navi = lNavigate.Text;

				hl.NavigateUrl = Navi.Replace("(Symbol)",Config.SymbolParameterName+"="+r);

				tdGallery.Controls.Add(hl);
				Literal l = new Literal();
				l.Text = "&nbsp;&nbsp;";
				tdGallery.Controls.Add(l);
				if ((i % Cols)==(Cols-1)) 
				{
					l = new Literal();
					l.Text = "<br><br>";
					tdGallery.Controls.Add(l);
				}
				i++;
			}
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
	}
}