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
	/// DBWebChart Demos
	/// </summary>
	public  partial class DBWebChart : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.DropDownList ddlWidth;
	
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

			if (tbCompare.Text!="")
				Compare +=";";
			Compare +=tbCompare.Text;
			
			if (!IsPostBack) 
			{
				string Code = Request.QueryString[Config.SymbolParameterName];
				if (Code!=null && Code!="")
					tbCode.Text = Request.QueryString[Config.SymbolParameterName];
			}

			cbRealTime.Visible = Config.RealtimeVisible();
			string RT = Config.UseRealtime(cbRealTime)?"1":"0";

			string Start;
			string End;
			string Cycle;
			Utils.PreRange(rblRange.SelectedItem.Value,out Start,out End,out Cycle);

			lCompare.Text = tbCode.Text;
			ibChart.ImageUrl = 
				"~/Chart.aspx?Provider="+Config.CustomChartDataManager+"&Code="+tbCode.Text.Trim()+
				"&Type="+rblType.SelectedItem.Value+
				"&Scale="+rblScale.SelectedItem.Value+
				"&MA="+MA+
				"&EMA="+EMA+
				"&IND="+Indicator+
				"&OVER="+Overlay+
				"&COMP="+Compare+
				"&Skin="+Skin+
				"&Size="+Size+
				"&RT="+RT+
				"&Start="+Start+
				"&End="+End+
				"&Cycle="+Cycle+
				"&Layout="+Config.LayoutForCustomChart+
				"&X="+Tools.ToIntDef(Request.Form[ibChart.ID+".x"],0)+
				"&Y="+Tools.ToIntDef(Request.Form[ibChart.ID+".y"],0)+
				"&His=";//(cbHisQuote.Checked?1:0)+
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			Utils.AddSkinToDropList(ddlSkin);
			try
			{
				ddlSize.SelectedValue = Config.DefaultChartWidth;
			} 
			catch
			{
			}
			InitializeComponent();
			base.OnInit(e);
		}
		
		private void InitializeComponent()
		{    

		}
		#endregion
	}
}
