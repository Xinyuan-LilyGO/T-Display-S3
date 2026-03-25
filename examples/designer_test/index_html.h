#pragma once
#include "Arduino.h"

static const char INDEX_HTML[] PROGMEM = R"html(
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Designer Image Test</title>
<style>
  body{font-family:sans-serif;max-width:520px;margin:2em auto;padding:0 1em;background:#111;color:#eee}
  h2{color:#4af}
  .drop{border:2px dashed #4af;border-radius:8px;padding:2em;text-align:center;cursor:pointer;margin-bottom:1em}
  .drop.over{background:#1a2a3a}
  input[type=file]{display:none}
  button{background:#4af;color:#000;border:none;padding:.5em 1.2em;border-radius:4px;cursor:pointer;font-size:1em}
  button.del{background:#f44;color:#fff;padding:.2em .6em;font-size:.85em}
  ul{list-style:none;padding:0}
  li{display:flex;align-items:center;justify-content:space-between;padding:.3em 0;border-bottom:1px solid #333}
  #status{color:#4f4;margin-top:.5em;min-height:1.2em}
  #pbar{display:none;width:100%;height:8px;background:#333;border-radius:4px;margin-top:.5em}
  #pfill{height:100%;width:0;background:#4af;border-radius:4px;transition:width .2s}
</style>
</head><body>
<h2>Designer Image Test</h2>
<div class="drop" id="drop" onclick="document.getElementById('fi').click()">
  Drop JPG/BMP here or <u>click to browse</u>
</div>
<input type="file" id="fi" accept="image/jpeg,image/bmp" multiple>
<div id="pbar"><div id="pfill"></div></div>
<div id="status"></div>
<h3>Stored images</h3>
<ul id="list"></ul>
<script>
function setStatus(msg,col){var s=document.getElementById('status');s.style.color=col||'#4f4';s.textContent=msg;}
function loadList(){
  fetch('/list').then(function(r){return r.json();}).then(function(files){
    var ul=document.getElementById('list');ul.innerHTML='';
    if(!files.length){ul.innerHTML='<li style="color:#888">No images yet</li>';return;}
    files.forEach(function(f){
      var li=document.createElement('li');
      li.innerHTML='<span>'+f+'</span><button class="del" onclick="delFile(\''+f+'\')">&#x2715;</button>';
      ul.appendChild(li);
    });
  });
}
function delFile(name){
  if(!confirm('Delete '+name+'?'))return;
  fetch('/delete?f='+encodeURIComponent(name)).then(function(r){return r.text();}).then(function(){loadList();setStatus(name+' deleted','#fa4');});
}
function uploadFiles(files){
  var i=0;
  function next(){
    if(i>=files.length){setStatus('All done!');loadList();return;}
    var f=files[i++];
    var fd=new FormData();fd.append('file',f);
    setStatus('Uploading '+f.name+'...');
    var xhr=new XMLHttpRequest();
    xhr.open('POST','/upload');
    xhr.upload.onprogress=function(e){if(e.lengthComputable){var pct=Math.round(e.loaded/e.total*100);document.getElementById('pfill').style.width=pct+'%';}};
    xhr.onload=function(){document.getElementById('pbar').style.display='none';document.getElementById('pfill').style.width='0';if(xhr.status===200){setStatus(f.name+' uploaded');next();}else{setStatus('Upload failed: '+xhr.responseText,'#f44');}};
    document.getElementById('pbar').style.display='block';
    xhr.send(fd);
  }
  next();
}
document.getElementById('fi').addEventListener('change',function(){uploadFiles(this.files);this.value='';});
var drop=document.getElementById('drop');
drop.addEventListener('dragover',function(e){e.preventDefault();this.classList.add('over');});
drop.addEventListener('dragleave',function(){this.classList.remove('over');});
drop.addEventListener('drop',function(e){e.preventDefault();this.classList.remove('over');uploadFiles(e.dataTransfer.files);});
loadList();
</script></body></html>
)html";
