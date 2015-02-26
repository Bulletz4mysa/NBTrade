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
using Easychart.Finance.DataProvider;
using EasyTools;

namespace WebDemos.DBDemos
{
	/// <summary>
	/// Summary description for FormulaValue.
	/// </summary>
	public  partial class FormulaValue : System.Web.UI.Page
	{

		private void Bind() 
		{
			DataTable dtFormulaValue = null;
			DataTable dtMain = null;
			DataRow drDate = DB.GetFirstRow("select Max(CalculateTime) from FormulaValue");
			if (drDate!=null)
			{
				object o = drDate[0];
				if (o is DateTime)
					lDate.Text = ((DateTime)o).ToString("MMM dd yyyy",DateTimeFormatInfo.InvariantInfo);
			}
			IDataManager idm = Utils.GetDataManager(Config.DefaultDataManager);

			dgList.VirtualItemCount = idm.SymbolCount("",null,null);// Tools.ToIntDef(drCount[0].ToString(),0);
				
			lTotal.Text = "Total :"+dgList.VirtualItemCount;
			dtMain = idm.GetSymbolList("",null,null,"",dgList.CurrentPageIndex*dgList.PageSize,dgList.PageSize);
			dtMain.PrimaryKey = new DataColumn[]{dtMain.Columns[0]};	
			if (dtMain.Rows.Count>0)
			{
				string s1 = dtMain.Rows[0][0].ToString();
				string s2 = dtMain.Rows[dtMain.Rows.Count-1][0].ToString();
				dtFormulaValue = DB.GetDataTable("select * from FormulaValue where QuoteCode>='"+s1+"' and  QuoteCode<='"+s2+"' order by QuoteCode");
			}
			
			string[] ss=Config.AutoPullFormulaData.Split(';');
			foreach(string s in ss)
				dtMain.Columns.Add(Utils.GetName(s));
			
			if (dtFormulaValue!=null)
			{
				foreach(DataRow dr in dtFormulaValue.Rows) 
				{
					DataRow drMain = dtMain.Rows.Find(dr["QuoteCode"]);
					if (drMain!=null)
					{
						object o = dr["FormulaValue"];
						double d = double.NaN;
						string s = "";
						if (o!=DBNull.Value)
							d = float.Parse(o.ToString());
						if (!double.IsNaN(d))
							s = d.ToString("f2");
						if (s!="")
							drMain[dr["FormulaName"].ToString()] = s;
					}
				}
			}

//			dgList.Columns.Clear();
//			foreach(DataColumn dc in dtMain.Columns)
//			{
//				BoundColumn bc = new BoundColumn();
//				bc.HeaderText = dc.ColumnName;
//				bc.DataField = dc.ColumnName;
//				bc.SortExpression = dc.ColumnName;
//				dgList.Columns.Add(bc);
//			}
//
			dgList.DataSource = dtMain;
			dgList.DataBind();
		}
	
		protected void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here
			if (!IsPostBack)
			{
				Bind();
			}
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			dgList.Columns.Clear();
			HyperLinkColumn hlc = new HyperLinkColumn();
			hlc.DataNavigateUrlField = "QuoteCode";
			hlc.DataNavigateUrlFormatString = lChartPage.Text+"?"+Config.SymbolParameterName+"={0}";
			hlc.DataTextField = "QuoteCode";
			hlc.HeaderText = "QuoteCode";
			dgList.Columns.Add(hlc);
			string[] ss = Config.AutoPullFormulaData.Split(';');
			for(int i=0; i<ss.Length; i++) 
			{
				BoundColumn bc = new BoundColumn();
				bc.DataField = Utils.GetName(ss[i]);
				bc.HeaderText = bc.DataField;
				bc.SortExpression = bc.DataField;
				//bc.DataFormatString = "{0:f2}";
				dgList.Columns.Add(bc);
			}

			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.dgList.PageIndexChanged += new System.Web.UI.WebControls.DataGridPageChangedEventHandler(this.dgList_PageIndexChanged);

		}
		#endregion

		private void dgList_PageIndexChanged(object source, System.Web.UI.WebControls.DataGridPageChangedEventArgs e)
		{
			dgList.CurrentPageIndex =e.NewPageIndex;
			Bind();
		}
	}
}
