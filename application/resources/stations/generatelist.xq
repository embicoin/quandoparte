<stations>
{
for $doc in ('nord.osm', 
             'centronord.osm', 
	     'centrosud.osm', 
	     'sud.osm', 
	     'sicilia.osm', 
	     'puglia.osm', 
	     'sardegna.osm')
for $node in doc($doc)//node
where $node/tag[@k='name'] and (
$node/tag[@k='operator'] and (
fn:contains($node//tag[@k='operator']/@v, 'RFI') or 
fn:contains($node//tag[@k='operator']/@v, 'Centostazioni') or 
fn:contains($node//tag[@k='operator']/@v, 'Grandi Stazioni')))
return 
<station>
<pos>{
data($node/@lon)
},{
data($node/@lat)
}
</pos>
<name>
{
data($node//tag[@k='name']/@v)
}
</name>
</station>
}
</stations>
