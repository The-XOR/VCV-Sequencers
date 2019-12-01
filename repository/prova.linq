<Query Kind="Program">
  <Connection>
    <ID>2c4e07d5-2625-4a62-9ac7-6f292cd6db93</ID>
    <Persist>true</Persist>
    <Server>.\cosmed</Server>
    <Database>tempdb</Database>
    <ShowServer>true</ShowServer>
  </Connection>
  <Reference>&lt;RuntimeDirectory&gt;\System.Globalization.dll</Reference>
  <Reference>&lt;RuntimeDirectory&gt;\System.Linq.dll</Reference>
  <Reference>&lt;RuntimeDirectory&gt;\System.Runtime.InteropServices.dll</Reference>
  <Reference>&lt;RuntimeDirectory&gt;\System.Windows.Forms.dll</Reference>
  <Namespace>System</Namespace>
  <Namespace>System.Diagnostics</Namespace>
  <Namespace>System.Drawing</Namespace>
  <Namespace>System.Globalization</Namespace>
  <Namespace>System.IO</Namespace>
  <Namespace>System.Security.Cryptography</Namespace>
  <Namespace>System.Security.Cryptography.X509Certificates</Namespace>
  <Namespace>System.Text</Namespace>
  <Namespace>System.Windows.Forms</Namespace>
</Query>

int raggio = 4;
int dist_h = 14;
int dist_v = 14;

void Main()
{
	List<int[]> percorsi = new List<int[]>() {
	 new int[] {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63}
	,new int[] {0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8,16,17,18,19,20,21,22,23,31,30,29,28,27,26,25,24,32,33,34,35,36,37,38,39,47,46,45,44,43,42,41,40,48,49,50,51,52,53,54,55,63,62,61,60,59,58,57,56}
	,new int[] {0,8,16,24,32,40,48,56,1,9,17,25,33,41,49,57,2,10,18,26,34,42,50,58,3,11,19,27,35,43,51,59,4,12,20,28,36,44,52,60,5,13,21,29,37,45,53,61,6,14,22,30,38,46,54,62,7,15,23,31,39,47,55,63}
	,new int[] {56,48,40,32,24,16,8,0,1,9,17,25,33,41,49,57,58,50,42,34,26,18,10,2,3,11,19,27,35,43,51,59,60,52,44,36,28,20,12,4,5,13,21,29,37,45,53,61,62,54,46,38,30,22,14,6,7,15,23,31,39,47,55,63}
	,new int[] {0,1,2,3,4,5,6,7,15,23,31,39,47,55,63,62,61,60,59,58,57,56,48,40,32,24,16,8,9,10,11,12,13,14,22,30,38,46,54,53,52,51,50,49,41,33,25,17,18,19,20,21,29,37,45,44,43,42,34,26,27,28,36,35}
	,new int[] {56,48,57,40,49,58,32,41,50,59,24,33,42,51,60,16,25,34,43,52,61,8,17,26,35,44,53,62,0,9,18,27,36,45,54,63,1,10,19,28,37,46,55,2,11,20,29,38,47,3,12,21,30,39,4,13,22,31,5,14,23,6,15,7}
	,new int[] {0,7,1,6,2,5,3,4,8,15,9,14,10,13,11,12,16,23,17,22,18,21,19,20,24,31,25,30,26,29,27,28,32,39,33,38,34,37,35,36,40,47,41,46,42,45,43,44,48,55,49,54,50,53,51,52,56,63,57,62,58,61,59,60}
	,new int[] {0,56,8,48,16,40,24,32,1,57,9,49,17,41,25,33,2,58,10,50,18,42,26,34,3,59,11,51,19,43,27,35,4,60,12,52,20,44,28,36,5,61,13,53,21,45,29,37,6,62,14,54,22,46,30,38,7,63,15,55,23,47,31,39}
	,new int[] {0,1,2,3,4,5,6,7,15,23,31,39,47,55,63,62,61,60,59,58,57,56,48,40,32,24,16,8,9,10,11,12,13,14,22,30,38,46,54,53,52,51,50,49,41,33,25,17,18,19,20,21,29,37,45,44,43,42,34,26,27,28,36,35}
	,new int[] {0,4,1,5,2,6,3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63,58,62,57,61,56,60,48,52,40,44,32,36,24,28,16,20,8,12,9,13,10,14,18,22,26,30,34,38,42,46,50,54,49,53,41,45,33,37,25,29,17,21}
	,new int[] { 0, 7, 9, 14, 18, 21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63, 1, 6, 10, 13, 19, 20, 43, 44, 50, 53, 57, 62, 8, 15, 17, 22, 26, 29, 34, 37, 41, 46, 48, 55, 2, 5, 11, 12, 51, 52, 58, 61, 16, 23, 25, 30, 33, 38, 40, 47, 3, 4, 59, 60, 24, 31, 32, 39 }
	,new int[] { 0, 7, 3, 4, 9, 14, 10, 13, 16, 23, 19, 20, 25, 30, 26, 29, 32, 39, 35, 36, 41, 46, 42, 45, 48, 55, 51, 52, 57, 58, 61, 62, 1, 6, 11, 12, 17, 22, 27, 28, 33, 38, 43, 44, 49, 54, 59, 60, 2, 5, 8, 15, 18, 21, 24, 31, 34, 37, 40, 47, 50, 53, 56, 63 }
	};


	int prcs=1;
	foreach(var p in percorsi)
	{
		string nome=$@"d:\temp\perc{prcs}.svg";
		preambolo(nome);

		for (int r = 0; r < 8; r++)
		{
			for (int c = 0; c < 8; c++)
			{
				cerchio(nome, c, r);
			}
		}
		percorsino(nome, p);

		chiusura(nome);
		prcs++;
	}
}

private void percorsino(string nome, int[] n)
{
	int id=1;
	for(int k = 0; k < n.Length-1; k++)
	{
		int c = n[k] % 8;
		int r = 7 - n[k] / 8;
		
		float from_cx = 20.0f + c * dist_h;
		float from_cy = 200.0f - r * dist_v;

		c = n[k+1] % 8;
		r = 7 - n[k+1] / 8;
		float to_cx = 20.0f + c * dist_h;
		float to_cy = 200.0f - r * dist_v;
 		string i=$"<path style = 'fill:none;stroke:#ff0000;stroke-width:0.5px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1' d='M {from_cx}, {from_cy} L {to_cx},{to_cy}' id='{1000 + id}' inkscape:connector-curvature = '0' />";
		File.AppendAllText(nome, i.Replace('\'', '"'));
	}
}

private void cerchio(string nome, int c, int r)
{
	int id = c+1+(r*8);
	int cx = 20 + c*dist_h;
	int cy = 200 - r *dist_v;
	string i = $"<circle style = 'opacity:1;fill:#000000;fill-opacity:1;stroke:none' id = 'path{id}' cx = '{cx}' cy = '{cy}' r = '{raggio}' />\n";
	File.AppendAllText(nome, i.Replace('\'', '"'));
}

private void chiusura(string nome)
{
	string i = @"
  </g>
</svg>
";
	File.AppendAllText(nome, i.Replace('\'', '"'));
}

private void preambolo(string nome)
{
string i = @"
<?xml version='1.0' encoding='UTF-8' standalone='no'?>
<!-- Created with Inkscape (http://www.inkscape.org/) -->

<svg
   xmlns:dc='http://purl.org/dc/elements/1.1/'
   xmlns:cc='http://creativecommons.org/ns#'
   xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'
   xmlns:svg='http://www.w3.org/2000/svg'
   xmlns='http://www.w3.org/2000/svg'
   xmlns:sodipodi='http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd'
   xmlns:inkscape='http://www.inkscape.org/namespaces/inkscape'
   width='210mm'
   height='297mm'
   viewBox='0 0 210 297'
   version='1.1'
   id='svg8'
   inkscape:version='0.92.2 (5c3e80d, 2017-08-06)'
   sodipodi:docname='cerchio.svg'>
  <defs
     id='defs2' />
  <sodipodi:namedview
     id='base'
     pagecolor='#ffffff'
     bordercolor='#666666'
     borderopacity='1.0'
     inkscape:pageopacity='0.0'
     inkscape:pageshadow='2'
     inkscape:zoom='0.35'
     inkscape:cx='-64.285714'
     inkscape:cy='560'
     inkscape:document-units='mm'
     inkscape:current-layer='layer1'
     showgrid='false'
     inkscape:window-width='1858'
     inkscape:window-height='1057'
     inkscape:window-x='3832'
     inkscape:window-y='-8'
     inkscape:window-maximized='1' />
  <metadata
     id='metadata5'>
    <rdf:RDF>
      <cc:Work
         rdf:about=''>
        <dc:format>image/svg+xml</dc:format>
        <dc:type
           rdf:resource='http://purl.org/dc/dcmitype/StillImage' />
        <dc:title></dc:title>
      </cc:Work>
    </rdf:RDF>
  </metadata>
  <g
     inkscape:label='Layer 1'
     inkscape:groupmode='layer'
     id='layer1'>
";
	File.WriteAllText(nome, i.Replace('\'', '"'));
}