using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using System.Reflection;
using System.IO;
using System.Web.Caching;
using Easychart.Finance;
using Easychart.Finance.DataProvider;

namespace WebDemos
{
	public  partial class Formula : System.Web.UI.Page
	{
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN：该调用是 ASP.NET Web 窗体设计器所必需的。
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// 设计器支持所需的方法 - 不要使用代码编辑器修改
		/// 此方法的内容。
		/// </summary>
		private void InitializeComponent()
		{    

		}
		#endregion

		protected void btnOK_Click(object sender, System.EventArgs e)
		{
			//Create a Formula namespace
			FormulaSpace fms = new FormulaSpace("FML");

			//Create a Formula program , set Formula name and script code on the fly
			FormulaProgram fp = new FormulaProgram();
			fp.Name = tbFormulaName.Text;
			fp.Code = tbProgramScript.Text;

			//Add the script program to the Formula namespace
			fms.Programs.Add(fp);

			//Add parameters to Formula program
			for(int i=1; i<5; i++) 
			{
				if (Request.Form["tbParamName"+i]!="") 
				{
					fp.Params.Add(new FormulaParam(
						Request.Form["tbParamName"+i],
						float.Parse(Request.Form["tbDefValue"+i]),
						float.Parse(Request.Form["tbMinValue"+i]),
						float.Parse(Request.Form["tbMaxValue"+i])));
				}
			}

			try 
			{
				//Compile the Formula script on the fly
				Assembly a = fms.CompileInMemory(HttpRuntime.BinDirectory);
				FormulaBase fb = FormulaBase.GetFormulaByName(a,fms.Name+"."+fp.Name);

				//Create YahooDataManager , Get stock data from yahoo.
				YahooDataManager ydm = new YahooDataManager();
				ydm.CacheRoot = HttpRuntime.AppDomainAppPath+"Cache\\";
				CommonDataProvider DataProvider = (CommonDataProvider)ydm[tbCode.Text];
			
				//Create financial chart instance
				FormulaChart fc = new FormulaChart();
				fc.AddArea("MAIN",3);
				fc.AddArea(fb);
				fc.DataProvider = DataProvider;
				fc.SetSkin(Config.DefaultSkin);


				//Show the temp image just created
				lChart.Text = "<img src=ImageFromCache.aspx?CacheId="+fc.SaveToImageStream(440,440,ImageFormat.Png,0,0)+">";
			}
			catch (FormulaErrorException fee)
			{
				//Show the compile result if the script has some errors
				lChart.Text = "<font color=red>";
				foreach(System.CodeDom.Compiler.CompilerError ce in fee.ces) 
				{
					FormulaProgram fpa = fms.GetProgramByLineNum(ce.Line);
					if (fpa!=null)
					{
						fpa.AdjustErrors(ce);
						lChart.Text += string.Format("Name:{0} line:{1} column:{2} error:{3} {4}<br>",fpa.Name,ce.Line,ce.Column,ce.ErrorNumber,ce.ErrorText);
					}
				}
				lChart.Text +="</font>";
			}
		}
	}
}
