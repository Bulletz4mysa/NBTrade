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
using System.Globalization;
using System.Threading;

namespace WebDemos.Admin
{
	/// <summary>
	/// Summary description for TestServer.
	/// </summary>
	public  partial class TestServer : System.Web.UI.Page
	{
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			lCulture.Text = "Culture:"+Thread.CurrentThread.CurrentCulture+" UICulture:"+Thread.CurrentThread.CurrentUICulture;
			if (lUTCNow!=null)
				lUTCNow.Text = DateTime.UtcNow.ToString();
			if (lTimeZone!=null) 
			{
				TimeZone tz = TimeZone.CurrentTimeZone;
				lTimeZone.Text = tz.StandardName+";"+ tz.GetUtcOffset(DateTime.Now);
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

		protected void btnTest_Click(object sender, System.EventArgs e)
		{
			lDateResult.Text =  DateTime.ParseExact(tbDate.Text,tbDateFormat.Text, DateTimeFormatInfo.InvariantInfo).ToString();
		}
	}
}
