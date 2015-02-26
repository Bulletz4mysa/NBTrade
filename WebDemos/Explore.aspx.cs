using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Web;
using System.Web.UI.WebControls;
using Msft = Microsoft.Web.UI.WebControls;
using System.Globalization;
using System.Text;
using Easychart.Finance;
using Easychart.Finance.DataProvider;

namespace WebDemos
{
	/// <summary>
	/// Financial Chart Indicator's Explore
	/// </summary>
	public  partial class Explore : System.Web.UI.Page
	{
		int Index;
		FormulaProgram CurrentProgram = null;
		Msft.TreeNode CurrentNode = null;
		string CurrentFullName;
		int CurrentId;
		//Random Rnd = new Random();
	
		private string Root() 
		{
			string s = HttpRuntime.AppDomainAppVirtualPath;
			if (s=="/")
				s = "";
			return s;
		}

        private void LoadToTree(Msft.TreeNode tn, FormulaParam fp) 
		{
			tn.Text = fp.Name;
		}

        private void LoadToTree(Msft.TreeNode tn, FormulaProgram p, string FullName)
		{
			tn.Text = p.Name;
			if (CurrentId==Index-1)
			{
				CurrentNode = tn;
				CurrentProgram = p;
				CurrentFullName = FullName;
			}
			foreach(FormulaParam fp in p.Params) 
			{
                Msft.TreeNode t = new Msft.TreeNode();
				t.ImageUrl = Root() +"/Images/Parameter.bmp";
				tn.Nodes.Add(t);
				LoadToTree(t,fp);
			}
		}

        private void LoadToTree(Msft.TreeNode tn, FormulaSpace fs, string FullName) 
		{
			tn.Text = fs.Name;
			foreach(FormulaSpace n in fs.Namespaces) 
			{
                Msft.TreeNode t = new Msft.TreeNode();
				t.ImageUrl = Root() + "/Images/Namespace.bmp";
				tn.Nodes.Add(t);
				if (n.GroupOnly)
					LoadToTree(t,n,FullName);
				else LoadToTree(t,n,FullName+"."+n.Name);
			}
			foreach(FormulaProgram p in fs.Programs) 
			{
                Msft.TreeNode t = new Msft.TreeNode();
				t.ImageUrl = Root() + "/Images/Program.bmp" ;

				t.NavigateUrl = "Explore.aspx?Id="+Index+++"&File="+ddlFormulaFile.SelectedItem.Value;
				
				tn.Nodes.Add(t);
				LoadToTree(t,p,FullName+"."+p.Name);
			}
		}

		private void LoadToTree(FormulaSpace fs) 
		{
			Index =0;
			FormulaTree.Nodes.Clear();
            FormulaTree.Nodes.Add(new Msft.TreeNode());
			LoadToTree(FormulaTree.Nodes[0],fs,"FML");
		}

		private string ReplaceMethodWithColor(string s)
		{
			string[] ss = FormulaBase.GetAllNativeMethods();
			return ReplaceMethodWithColor(s,"#c08000",FormulaBase.GetAllNativeMethods(),true);
		}

		private string ReplaceMethodWithColor(string s,string ReplaceColor,string[] ss,bool Link)
		{
			Hashtable ht = new Hashtable();
			foreach(string sss in ss)
				ht[sss] = 1;

			s +=".";
			StringBuilder sb = new StringBuilder();
			int j = -1;
			for(int i=0; i<s.Length; i++)
			{
				if (char.IsLetterOrDigit(s[i]))
				{
					if (j<0)
						j = i;
				}
				else 
				{
					if (j>=0)
					{
						string r = s.Substring(j,i-j).ToUpper();
						if (ht[r]!=null) 
						{
							if (Link)
								sb.Append("<a href=FormulaHelp.aspx?Filter="+r+">");
							sb.Append("<font color="+ReplaceColor+"><B>"+r+"</font></B>");
							if (Link)
								sb.Append("</a>");
						}
						else sb.Append(r);
						j = -1;
					}
					sb.Append(s[i]);
				}
			}
			return sb.ToString(0,sb.Length-1);
		}

		private void BindData() 
		{
			string F = Request.QueryString["File"];
			if (F!=null && F!="") 
			{
				ListItem li = ddlFormulaFile.Items.FindByText(F);
				if (li!=null) 
				{
					foreach(ListItem l in ddlFormulaFile.Items)
						l.Selected = false;
					li.Selected = true;
				}
			}
			string FileName = Config.PluginsDirectory+ddlFormulaFile.SelectedItem.Value;
			FormulaSpace fs = FormulaSpace.Read(FileName);

			string Id = Request.Params["Id"];
			if (Id==null || Id=="")
				CurrentId = -1;
			else CurrentId = int.Parse(Id);
			LoadToTree(fs);
			if (CurrentProgram==null)
			{
                Msft.TreeNode t = FormulaTree.Nodes[0];
				while (t.Nodes.Count>0) 
				{
					t.Expanded = true;
					t = t.Nodes[0];
				}
				tdChart.Visible = false;
			} 
			else 
			{
                for (object o = CurrentNode; !(o is Msft.TreeView); o = ((Msft.TreeNode)o).Parent)
                    ((Msft.TreeNode)o).Expanded = true;

				FormulaTree.SelectedNodeIndex = CurrentNode.GetNodeIndex();

				lParam.Text = "<table border=1><tr><td>Name</td><td>Default Value</td><td>Minimum Value</td><td>Maxmum Value</td></tr>";
				foreach(FormulaParam fpm in CurrentProgram.Params) 
				{
					lParam.Text +="<tr><td>";
					lParam.Text +=fpm.Name +"</td><td>";
					string Value = fpm.DefaultValue;
					string r = "__Param"+fpm.Name;
					if (Request.Form[r]!=null && Request.Form[r]!="")
						Value = Request.Form[r];

					lParam.Text +="<input Name="+r+" value="+Value+"></td><td>";
					lParam.Text +=fpm.MinValue+"</td><td>";
					lParam.Text +=fpm.MaxValue+"</td></tr>";
				}
				lParam.Text +="</table><br>";
				lName.Text = CurrentProgram.Name;
				lFullName.Text = CurrentProgram.FullName;
				lDescription.Text = "<br>"+CurrentProgram.Description.Replace("\n","<br>");
				lCode.Text = "<br>"+
					ReplaceMethodWithColor(
						ReplaceMethodWithColor(Server.HtmlEncode(CurrentProgram.Code),"#008000",CurrentProgram.Params.GetParamList(),false)
					).Replace("\n","<br>");
				lMainView.Visible = CurrentProgram.IsMainView;
			}
		}

		protected void Page_Load(object sender, System.EventArgs e)
		{
			if (!IsPostBack)
				BindData();
			else if (Request.Form["__EVENTTARGET"]=="ddlFormulaFile")
				Response.Redirect("Explore.aspx?File="+ddlFormulaFile.SelectedItem.Text);
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
			BindData();

			FormulaBase fb = FormulaBase.GetFormulaByName(CurrentFullName);
			foreach(string s in Request.Form)
				if (s.StartsWith("__Param")) 
					fb.SetParam(s.Substring(7),Request.Form[s]);

			YahooDataManager ydm = new YahooDataManager();
			ydm.CacheRoot = HttpRuntime.AppDomainAppPath+"Cache\\";
			CommonDataProvider DataProvider = (CommonDataProvider)ydm[tbCode.Text];

			FormulaChart fc = new FormulaChart();
			fc.AddArea("MAIN",2);
			if (CurrentProgram.IsMainView)
				fc[0].AddFormula(fb);
			else 
				fc.AddArea(fb);
			fc.LatestValueType = LatestValueType.StockOnly;

			fc.SetSkin(Config.DefaultSkin);

			try 
			{
				fc.DataProvider = DataProvider;
				FormulaData fdDate = DataProvider["DATE"];
				string s = tbCode.Text;
				if (fdDate.Length>0)
					s +=" ("+DateTime.FromOADate(fdDate[fdDate.Length-1]).ToString("yyyy-MMM-dd",DateTimeFormatInfo.InvariantInfo)+")";

				fc[0].Formulas[0].Name = s;
				fc[0].Formulas[0].TextInvisible = false;
				fc.StartTime = DateTime.Now.AddMonths(-8);
				fc.EndTime =  DateTime.Now;
			}
			catch (Exception ee) 
			{
				Bitmap b = new Bitmap(640,480);
				Graphics g = Graphics.FromImage(b);
				g.FillRectangle(Brushes.WhiteSmoke,0,0,640,480);
				g.DrawRectangle(Pens.Black,0,0,639,479);
				g.DrawString(ee.Message,new Font("verdana",10),Brushes.Red,1,1);
			}

			lChart.Text = "<img src=ImageFromCache.aspx?CacheId="+fc.SaveToImageStream(640,400,ImageFormat.Png,0,0)+">";
		}

		protected void ddlFormulaFile_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		}
	}
}
