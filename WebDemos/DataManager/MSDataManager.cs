using System;
using System.Web;

namespace WebDemos
{
	/// <summary>
	/// Summary description for MSDataManager.
	/// </summary>
	public class MSDataManager : Easychart.Finance.DataProvider.MSDataManager
	{
		public MSDataManager():base(HttpRuntime.AppDomainAppPath+"MetaDaily")
		{
		}
	}
}

