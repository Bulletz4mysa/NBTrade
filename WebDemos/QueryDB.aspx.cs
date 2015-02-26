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
using EasyDb;

namespace WebDemos
{
	/// <summary>
	/// Summary description for QueryDB.
	/// </summary>
	public class QueryDB : System.Web.UI.Page
	{
		protected System.Web.UI.WebControls.Label lQueryString;
		protected System.Web.UI.WebControls.Button tnOK;
		protected System.Web.UI.WebControls.DataGrid dgResult;
		protected System.Web.UI.WebControls.TextBox tbSql;
	
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
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
			this.tnOK.Click += new System.EventHandler(this.tnOK_Click);
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion

		private void tnOK_Click(object sender, System.EventArgs e)
		{
			dgResult.DataSource = DB.GetDataTable(tbSql.Text);
			dgResult.DataBind();
		}
	}
}
