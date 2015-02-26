namespace WebDemos.DBDemos
{
	using System;
	using System.Web;
    using System.Web.ClientServices;
    /// <summary>
	///		Summary description for mySelectDateRange.
	/// </summary>
	public partial class SelectDateRange : System.Web.UI.UserControl
	{

		protected string AllCycles;

		private static string GroupName = "RangeType";

		public  string Start;
		public  string End;
		public  string Cycle;

		public  string RType = "0";
		public string IsCheck(int Id) 
		{
			return RType == Id.ToString()?" checked":"";
		}

		private void Page_Load(object sender, System.EventArgs e)
		{

            Page.ClientScript.RegisterClientScriptBlock(typeof(string),"clientScript", "<script language=JavaScript> function UpdateRangeType(newOption) {"
				+ "document.wfCustomChart."+GroupName+"[newOption].checked = true;}" // GroupName
				+ "</script>");

			ddlCycle.Attributes["OnChange"] ="UpdateRangeType(1)";
			dpStart.ChangeScript = "UpdateRangeType(1)";
			dpEnd.ChangeScript = "UpdateRangeType(1)";
		}

		public  void BindValue()
		{
			Start = dpStart.Date.ToString("yyyyMMdd"); 
			End = dpEnd.Date.ToString("yyyyMMdd");
			Cycle = HttpContext.Current.Request.Form[ddlCycle.UniqueID];
            string Range = HttpContext.Current.Request.Form[rblRange.UniqueID];
			if (Range==null)
				Range = rblRange.SelectedValue;

            if (HttpContext.Current.Request.Form[GroupName] != null)
                RType = HttpContext.Current.Request.Form[GroupName];
            string __EVENTTARGET = HttpContext.Current.Request.Form["__EVENTTARGET"];
			if (__EVENTTARGET!=null && __EVENTTARGET.StartsWith(rblRange.ClientID))
			{
				RType = "0";
			}

			if (RType == "0" && Range!=null)
				Utils.PreRange(Range,out Start,out End,out Cycle);
			base.DataBind ();
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			InitializeComponent();
			BindValue();
			base.OnInit(e);
		}
		
		/// <summary>
		///		Required method for Designer support - do not modify
		///		the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.Load += new System.EventHandler(this.Page_Load);

		}
		#endregion
	}
}