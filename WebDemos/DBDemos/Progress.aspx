<%@ Page language="c#" Inherits="WebDemos.DBDemos.Progress" CodeBehind="Progress.aspx.cs" %>
<HTML>
	<HEAD>
		<title>Scanning progress</title>
		<LINK href="../m.css" type="text/css" rel="stylesheet">
			<META HTTP-EQUIV="REFRESH" CONTENT="1">
	</HEAD>
	<body>
		<form method="post" runat="server">
			<br>
			<br>
			<br>
			<table width="100%">
				<tr>
					<td>
						<center>
							<table>
								<tr>
									<td valign="top">
										<asp:label id="lCondition" runat="server" EnableViewState="False"></asp:label>
									</td>
									<td valign="top">
										<asp:label id="lTotal" runat="server"></asp:label>
									</td>
									<td valign="top">
										<asp:Label id="lProgress" runat="server"></asp:Label><br>
									</td>
								</tr>
							</table>
						</center>
					</td>
				</tr>
			</table>
		</form>
	</body>
</HTML>
