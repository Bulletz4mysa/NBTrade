<%@ Reference Page="~/Formula.aspx" %>
<%@ Control Language="c#" Inherits="WebDemos.DBDemos.SelectFormula" CodeFile="SelectFormula.ascx.cs" %>
Formulas:
<asp:DropDownList id="ddlFormula" runat="server" AutoPostBack="True"></asp:DropDownList>
<br>
<table border="1" cellpadding="2" cellspacing="0">
	<tr>
		<td>
			<B>Full Name</B>
		</td>
		<td>
			<asp:Label id="lFullName" runat="server"></asp:Label>
		</td>
	</tr>
	<tr>
		<td>
			<B>Script</B>
		</td>
		<td>
			<asp:Label id="lCode" runat="server" CssClass="Script"></asp:Label>
		</td>
	</tr>
	<tr>
		<td>
			<B>Description</B>
		</td>
		<td>
			<asp:Label id="lDescription" runat="server"></asp:Label>
		</td>
	</tr>
</table>
<b>Parameters</b>
<asp:Label id="lParam" runat="server"></asp:Label>
