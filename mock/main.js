var exec = require('child_process').exec

setInterval(function(){
	 exec('cmdmp3 ring.mp3', function(err){
	 	if(err){
	 		console.log(err);
	 	}
	 })
}, 3000);
