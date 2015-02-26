using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Drawing.Imaging;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using Easychart.Finance;
using Easychart.Finance.DataProvider;
using EasyTools;

namespace WebDemos.DBDemos
{
	/// <summary>
	/// Summary description for Compare3.
	/// </summary>
	public  partial class Compare3 : System.Web.UI.Page
	{
        
		private void BindLookup(DropDownList ddl,DataTable dt,int Index,string Default) 
		{
			if (dt.Rows.Count>0)
			{
				ddl.DataSource = dt;
				if (Index>=dt.Rows.Count)
					Index = dt.Rows.Count-1;

				ddl.DataBind();
				ListItem li = ddl.Items.FindByValue(Default);
				if (li!=null)
					ddl.SelectedValue = li.Value;
				else ddl.SelectedValue = dt.Rows[Index][0].ToString();
			}
		}
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			if (!IsPostBack) 
			{
				SelectDateRange.rblRange.SelectedValue = "max";
				SelectDateRange.BindValue();
				DataTable dt = DB.GetDataTable("select QuoteCode,QuoteName from stockdata where Exchange='Economic'");
				BindLookup(ddlSymbol1,dt,0,lDefault1.Text);
				DataRow dr = dt.NewRow();
				dr[0] = "";
				dr[1] = "N/A";
				dt.Rows.InsertAt(dr,0);
				BindLookup(ddlSymbol2,dt,0,"");
				BindLookup(ddlSymbol3,dt,0,"");
			}

			string Scale =rblScale.SelectedItem.Value; 
			string Skin = Config.DefaultSkin;

			ibChart.ImageUrl = "../Chart.aspx?Provider=DB&Code="+
				ddlSymbol1.SelectedValue+
				"&Over=EXTEND.COMPARE("+ddlSymbol2.SelectedValue+");EXTEND.COMPARE("+ddlSymbol3.SelectedValue+")"+
				"&Scale="+Scale+
				"&Skin="+Skin+
				"&Size=700*500"+
				"&Start="+SelectDateRange.Start+
				"&End="+SelectDateRange.End+
				"&Cycle="+SelectDateRange.Cycle+
				"&Type=1"+
				"&His=0"+
				"&X="+Tools.ToIntDef(Request.Form[ibChart.ID+".x"],0)+
				"&Y="+Tools.ToIntDef(Request.Form[ibChart.ID+".y"],0);
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
