using Easychart.Finance;
using Easychart.Finance.DataProvider;
namespace FML.TDCOMBO
{
	public class TDCombo:FormulaBase
	{
		public TDCombo():base()
		{
		}
	
		public override FormulaPackage Run(IDataProvider DP)
		{
			this.DataProvider = DP;
			FormulaData NONAME0=ASKVOL(9);
			return new FormulaPackage(new FormulaData[]{NONAME0},"");
		}
	
		public override string LongName
		{
			get{return "Tom Demark";}
		}
	
		public override string Description
		{
			get{return "";}
		}
	} //class TDCombo

} // namespace FML.TDCOMBO
