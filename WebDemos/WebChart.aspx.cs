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

namespace WebDemos
{
	/// <summary>
	/// WebChart Demos
	/// </summary>
	public partial  class WebChart : System.Web.UI.Page
	{
	
		private string Selected(CheckBoxList cbl) 
		{
			string result = "";
			foreach(ListItem li in cbl.Items) 
				if (li.Selected)
				{
					if (result!="") result+=";";
					result+=li.Value;
				}
			return result;
		}

		protected void Page_Load(object sender, System.EventArgs e)
		{
			string MA = Selected(cblMA);
			string EMA = Selected(cblEMA);
			string Indicator = Selected(cblIndicators);
			string Overlay = Selected(cblOverlays);
			string Compare = Selected(cblCompare);
			
			string Skin = ddlSkin.SelectedItem.Value;
			
			string Size = ddlSize.SelectedItem.Value;
			string Width = ddlWidth.SelectedItem.Value;
			if (tbCompare.Text!="")
				Compare +=";";
			Compare +=tbCompare.Text;

			if (!IsPostBack && Request.QueryString[Config.SymbolParameterName]!=null)
				tbCode.Text = Request.QueryString[Config.SymbolParameterName];

			lCompare.Text = tbCode.Text;

			string Start;
			string End;
			string Cycle;
			Utils.PreRange(rblRange.SelectedItem.Value,out Start,out End,out Cycle);

			lChart.Text = "<img src=Chart.aspx?Provider="+Config.WebChartDataManager+"&Code="+tbCode.Text+
				"&Type="+rblType.SelectedItem.Value+
				"&Scale="+rblScale.SelectedItem.Value+
				//"&Range="+rblRange.SelectedItem.Value+
				"&Start="+Start+
				"&End="+End+
				"&Cycle="+Cycle+
				"&MA="+MA+
				"&EMA="+EMA+
				"&IND="+Indicator+
				"&OVER="+Overlay+
				"&COMP="+Compare+
				"&Skin="+Skin+
				"&Size="+Size+
				"&Layout="+Config.LayoutForCustomChart+
				"&Width="+Width+
				">";
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			Utils.AddSkinToDropList(ddlSkin);
			InitializeComponent();
			base.OnInit(e);
		}
		
		private void InitializeComponent()
		{    

		}
		#endregion
	}
}