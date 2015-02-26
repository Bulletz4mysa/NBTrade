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
using EasyTools;
using Easychart.Finance.DataProvider;

namespace WebDemos.DBDemos
{
	/// <summary>
	/// Summary description for Lookup.
	/// </summary>
	public  partial class Lookup : System.Web.UI.Page
	{
	
		private void Bind()
		{
			IDataManager idm = Utils.GetDataManager(Config.DefaultDataManager);
			DataTable dt = idm.Exchanges;
			ddlExchange.DataSource = dt;
			ddlExchange.DataBind();
		}

		protected void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			if (!IsPostBack)
				Bind();
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

		protected void btnSearch_Click(object sender, System.EventArgs e)
		{
			if (tbKey!=null) 
			{
				string Exchange = ddlExchange.SelectedItem.Value;
				Exchange = "&E="+Exchange;
				Response.Redirect("StockList.aspx?Key="+tbKey.Text+Exchange);
			}
		}
	}
}
