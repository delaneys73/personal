<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<html>
<head>
<title>Delaney Family Christmas Lights</title>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.0/jquery.min.js"></script>
<script type="text/javascript">
	jQuery(document).ready(function(e){
		setTimeout(function(){
			jQuery(".message").hide('slow');
			},3000);
	});
</script>
<script type="text/javascript" src="/scripts/snowflakes.js"></script>
<link rel="stylesheet" href="/styles/style.css">
<script type="text/javascript">

</script>
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
</head>
<body>
<c:if test="${message ne ''}">
<h1 class="message">${message}</h1>
</c:if>
<div class="tabs">
	<div class="tab selectedTab">Music</div>
	<div class="tab"><a href="/app/lights.page">Lights</a></div>
</div>
<div class="body">
<div class="intro">
Welcome to the Delaney family, interactive Christmas Lights display. Select one of the links below to start the playback of a musical sequence.
Note: Only one sequence can play at a time. Please wait before starting another sequence. 
</div>
<h2>Merry Christmas</h2>

<ul class="musicList">
	<c:forEach var="song" items="${music}">
	  <li><a href="/app/play.action/${song}.mp3">${song}</a></li>
	</c:forEach>
</ul>
</body>
</body>
</html>