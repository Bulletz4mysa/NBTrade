using System;
using System.Web;
using System.Data;
using EasyTools;

namespace WebDemos
{
	/// <summary>
	/// IHttpHandler for authorization
	/// </summary>
	public class McwytechAuth:IHttpModule
	{
		public McwytechAuth()
		{
		}
		#region IHttpModule Members

		public void Init(HttpApplication context)
		{
			context.AuthorizeRequest+=new EventHandler(context_AuthorizeRequest);
		}

		public void Dispose()
		{
			// TODO:  Add McwytechAuth.Dispose implementation
		}

		#endregion

		private void context_AuthorizeRequest(object sender, EventArgs e)
		{
			HttpContext context = ((HttpApplication)sender).Context;
			if (context.Request.RawUrl.ToLower().IndexOf(".aspx")>0)
			{
				string Symbol = context.Request.QueryString["Symbol"];
				if (Symbol!=null && Symbol!="")
				{
					HttpCookie hc = context.Request.Cookies["IISPROTECTLOGIN"];
					if (hc!=null)
					{
						string User = hc["USER"];
						if (User!=null && User!="")
						{
							DataRow dr = DB.GetFirstRow(
								"SELECT UserID, Exchange FROM z_UserAccExchange WHERE userid='"+User
								+"' AND exchange =(SELECT distinct exchange FROM z_config WHERE symbol='"+Symbol+"')");
							if (dr!=null)
								return;
						}
					}
					context.Response.Redirect("noaccess.asp",true);
				}
			}
		}
	}
}