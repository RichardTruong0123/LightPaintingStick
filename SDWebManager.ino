/*
  Light Painting Stick integrated SD Web Manager
  Uses the already-mounted global SD filesystem. Do NOT call SPI.begin() or SD.begin() here.
  WebServer is serviced from the main loop, alongside LVGL.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SD.h>
#include <lvgl.h>
#include <Globals.h>

static WebServer sdWebServer(80);
static const char *SD_WEB_AP_PREFIX = "LightPaintingStick-";
static String sdWebApSsid;
static const char *SD_WEB_AP_PASSWORD = "PaintLight0123"; // Change before sharing device
static bool sdWebStarted = false;
volatile bool sdWebBusy = false;
static File sdWebUploadFile;
static String sdWebUploadPath;
static bool sdWebUploadFailed = false;
static String sdWebUploadError;
static size_t sdWebUploadBytes = 0;
static uint32_t sdWebLastLvglService = 0;

// Keep this page small; it talks to the JSON endpoints below.
static const char SD_WEB_HTML[] PROGMEM = R"SDWEB(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Light Painting Stick SD Manager</title><style>
*{box-sizing:border-box}body{margin:0;background:#f2f5f8;color:#17212b;font:15px/1.45 system-ui,Segoe UI,Arial,sans-serif}header{background:#172536;color:white;padding:18px}header,main{padding-left:max(16px,calc((100% - 900px)/2));padding-right:max(16px,calc((100% - 900px)/2))}h1{font-size:21px;margin:0 0 4px}header p{margin:0;color:#d4deea}main{padding-top:18px;padding-bottom:28px}.card{background:white;border:1px solid #dce3eb;border-radius:12px;padding:16px;margin-bottom:14px}.row{display:flex;gap:8px;align-items:center;flex-wrap:wrap}button{border:0;border-radius:8px;padding:10px 13px;background:#1769aa;color:white;font:inherit;cursor:pointer}button.secondary{background:#e8edf4;color:#172536}button.danger{background:#b42318}button:disabled{opacity:.5}input{font:inherit;max-width:100%;padding:9px;border:1px solid #cbd5e1;border-radius:8px}#folder{flex:1;min-width:160px}#status{white-space:pre-wrap;color:#526173;margin-top:10px;overflow-wrap:anywhere}.muted{font-size:13px;color:#64748b}table{width:100%;border-collapse:collapse;margin-top:12px}td,th{text-align:left;padding:10px 6px;border-bottom:1px solid #e6ebf1;overflow-wrap:anywhere}th{font-size:12px;color:#64748b}td:last-child{text-align:right;white-space:nowrap}.link{border:0;background:transparent;color:#1769aa;padding:0;font-weight:600;text-align:left}.progress{height:9px;background:#e6ebf1;border-radius:8px;overflow:hidden;margin-top:10px}.bar{height:100%;width:0;background:#16834a}
.modal{position:fixed;inset:0;z-index:1000;background:rgba(10,18,28,.82);display:none;align-items:center;justify-content:center;padding:18px}
.modal.open{display:flex}.modal-panel{position:relative;background:#fff;border-radius:12px;padding:14px;max-width:min(100%,1000px);max-height:100%;width:fit-content;box-shadow:0 12px 40px rgba(0,0,0,.3)}
.modal-head{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:10px}.modal-title{font-weight:650;overflow-wrap:anywhere}.modal-close{background:#172536;padding:7px 11px}
#previewImage{display:block;max-width:min(88vw,960px);max-height:76vh;width:auto;height:auto;object-fit:contain;background:#eef2f6;border-radius:6px}
.preview-error{display:none;color:#b42318;padding:18px}.preview-btn{background:transparent;color:#1769aa;padding:0;font-weight:600;text-align:left}
 @media(max-width:540px){.card{padding:12px}td:last-child button{padding:7px}}
</style></head><body><header><h1>Light Painting Stick</h1><p>SD card file manager · BMP upload and folders</p></header><main>
<section class="card"><div class="row"><button class="secondary" onclick="up()">↑ Parent</button><button class="secondary" onclick="load()">Refresh</button><b id="pathLabel">/</b></div><div id="status" role="status">Loading SD card…</div></section>
<section class="card"><h2>Upload BMP images</h2><div class="row"><input id="files" type="file" accept=".bmp,image/bmp" multiple><button id="uploadBtn" onclick="upload()">Upload selected</button></div><div class="progress"><div class="bar" id="bar"></div></div><p class="muted">Files are uploaded to the folder currently open. Do not start playback or conversion during upload.</p></section>
<section class="card"><h2>Create folder</h2><form class="row" onsubmit="mkdir(event)"><input id="folder" maxlength="48" placeholder="Folder name" required><button>Create folder</button></form></section>
<section class="card"><h2>SD card contents</h2><div style="overflow-x:auto"><table><thead><tr><th>Name</th><th>Type / size</th><th>Action</th></tr></thead><tbody id="listing"><tr><td colspan="3">Loading…</td></tr></tbody></table></div></section>
</main>
<div id="previewModal" class="modal" role="dialog" aria-modal="true" aria-labelledby="previewTitle" onclick="if(event.target===this)closePreview()">
  <div class="modal-panel">
    <div class="modal-head"><div id="previewTitle" class="modal-title">BMP preview</div><button class="modal-close" onclick="closePreview()" aria-label="Close preview">Close ✕</button></div>
    <img id="previewImage" alt="BMP image preview">
    <div id="previewError" class="preview-error">This BMP could not be displayed.</div>
  </div>
</div>
<script>
let path='/';const $=id=>document.getElementById(id);function status(s){$('status').textContent=s}function enc(s){return encodeURIComponent(s)}function size(n){return n<1024?n+' B':n<1048576?(n/1024).toFixed(1)+' KB':(n/1048576).toFixed(2)+' MB'}function join(a,b){return a==='/'?'/'+b:a+'/'+b}
async function api(url,opt){let r=await fetch(url,opt);let j=await r.json().catch(()=>({error:'Invalid server response'}));if(!r.ok||j.ok===false)throw Error(j.error||('HTTP '+r.status));return j}
async function load(){ $('pathLabel').textContent=path;$('listing').innerHTML='<tr><td colspan="3">Loading…</td></tr>';try{let j=await api('/api/list?path='+enc(path));let es=j.entries||[];es.sort((a,b)=>(b.dir-a.dir)||a.name.localeCompare(b.name));$('listing').innerHTML='';if(!es.length)$('listing').innerHTML='<tr><td colspan="3">Folder is empty.</td></tr>';for(let e of es){let tr=document.createElement('tr'),n=document.createElement('td'),meta=document.createElement('td'),act=document.createElement('td');if(e.dir){let b=document.createElement('button');b.className='link';b.textContent='📁 '+e.name;b.onclick=()=>{path=join(path,e.name);load()};n.append(b);meta.textContent='Folder';}else{let preview=document.createElement('button');preview.className='preview-btn';preview.textContent='🖼 '+e.name;preview.onclick=()=>openPreview(e.name);n.append(preview);meta.textContent=size(e.size);}let d=document.createElement('button');d.className='danger';d.textContent='Delete';d.onclick=()=>del(e.name,e.dir);act.append(d);tr.append(n,meta,act);$('listing').append(tr)}status('SD card ready · '+es.length+' item(s)');}catch(e){$('listing').innerHTML='<tr><td colspan="3">Unable to read SD contents.</td></tr>';status('Error: '+e.message)}}
function openPreview(name){
  const modal=$('previewModal'),img=$('previewImage'),err=$('previewError');
  $('previewTitle').textContent=name;err.style.display='none';img.style.display='block';
  img.onload=()=>{err.style.display='none';img.style.display='block'};
  img.onerror=()=>{img.style.display='none';err.style.display='block'};
  img.src='/api/preview?path='+enc(join(path,name))+'&t='+Date.now();
  modal.classList.add('open');document.body.style.overflow='hidden';
}
function closePreview(){
  $('previewModal').classList.remove('open');document.body.style.overflow='';
  $('previewImage').removeAttribute('src');
}
document.addEventListener('keydown',e=>{if(e.key==='Escape')closePreview()});
function up(){if(path!=='/'){let i=path.lastIndexOf('/');path=i<=0?'/':path.slice(0,i);load()}}
async function mkdir(ev){ev.preventDefault();let n=$('folder').value.trim();if(!n)return;try{await api('/api/mkdir',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'path='+enc(path)+'&name='+enc(n)});$('folder').value='';status('Folder created.');load()}catch(e){status('Create folder failed: '+e.message)}}
async function del(name,dir){if(!confirm('Delete '+(dir?'folder':'file')+' "'+name+'"? Folders must be empty.'))return;try{await api('/api/delete',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'path='+enc(join(path,name))});status('Deleted '+name);load()}catch(e){status('Delete failed: '+e.message)}}
async function upload(){let fs=[...$('files').files];if(!fs.length){status('Choose one or more BMP files first.');return}let btn=$('uploadBtn');btn.disabled=true;try{for(let i=0;i<fs.length;i++){let f=fs[i];if(!f.name.toLowerCase().endsWith('.bmp'))throw Error('Only .bmp files are accepted: '+f.name);status('Uploading '+(i+1)+'/'+fs.length+': '+f.name);$('bar').style.width='0%';await new Promise((resolve,reject)=>{let x=new XMLHttpRequest();x.open('POST','/api/upload?path='+enc(path)+'&name='+enc(f.name));x.upload.onprogress=e=>{if(e.lengthComputable)$('bar').style.width=(100*e.loaded/e.total)+'%'};x.onload=()=>{let j={};try{j=JSON.parse(x.responseText)}catch{}if(x.status>=200&&x.status<300&&j.ok!==false)resolve();else reject(Error(j.error||'HTTP '+x.status))};x.onerror=()=>reject(Error('Network error'));let fd=new FormData();fd.append('file',f,f.name);x.send(fd)})}status('Upload complete.');$('files').value='';await load()}catch(e){status('Upload failed: '+e.message);load()}finally{btn.disabled=false}}
load();</script></body></html>
)SDWEB";

static String sdWebJsonEscape(const String &s) {
  String out; out.reserve(s.length()+8);
  for (size_t i=0;i<s.length();++i) { char c=s[i]; if(c=='"'||c=='\\'){out+='\\';out+=c;} else if(c=='\n')out+="\\n"; else if(c=='\r')out+="\\r"; else if((uint8_t)c>=0x20)out+=c; }
  return out;
}
static String sdWebNormalizePath(String path) {
  path.trim(); if(!path.length()) return "/"; path.replace('\\','/');
  if(!path.startsWith("/")) path="/"+path;
  while(path.indexOf("//")>=0) path.replace("//","/");
  if(path.indexOf("..")>=0 || path.indexOf(':')>=0) return "";
  while(path.length()>1 && path.endsWith("/")) path.remove(path.length()-1);
  return path;
}
static bool sdWebSafeName(String name) {
  if(!name.length() || name=="." || name==".." || name.indexOf('/')>=0 || name.indexOf('\\')>=0 || name.indexOf(':')>=0) return false;
  for(size_t i=0;i<name.length();++i) { char c=name[i]; if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' '||c=='.'||c=='_'||c=='-')) return false; }
  return true;
}
static void sdWebSendError(int code, const String &msg) {
  sdWebServer.send(code,"application/json; charset=utf-8",String("{\"ok\":false,\"error\":\"")+sdWebJsonEscape(msg)+"\"}");
}
static void sdWebHandleRoot() { sdWebServer.sendHeader("Cache-Control","no-store"); sdWebServer.send_P(200,"text/html; charset=utf-8",SD_WEB_HTML); }
static void sdWebHandleList() {
  if(!sdCardAvailable) { sdWebSendError(503,"SD card is not mounted"); return; }
  String path=sdWebNormalizePath(sdWebServer.arg("path")); if(!path.length()){sdWebSendError(400,"Invalid path");return;}
  sdWebBusy=true; File dir=SD.open(path); if(!dir||!dir.isDirectory()){if(dir)dir.close();sdWebBusy=false;sdWebSendError(404,"Folder not found");return;}
  String json="{\"ok\":true,\"entries\":["; bool first=true; File f=dir.openNextFile();
  while(f){String name=String(f.name()); int slash=name.lastIndexOf('/'); if(slash>=0)name=name.substring(slash+1); bool isDir=f.isDirectory(); String lower=name;lower.toLowerCase(); bool show=isDir||lower.endsWith(".bmp"); if(show){if(!first)json+=',';first=false;json+="{\"name\":\""+sdWebJsonEscape(name)+"\",\"dir\":"+(isDir?"true":"false")+",\"size\":"+String((uint32_t)f.size())+"}";} f.close(); if(json.length()>24000)break; f=dir.openNextFile(); }
  dir.close();sdWebBusy=false;json+="]}";sdWebServer.send(200,"application/json; charset=utf-8",json);
}
static void sdWebHandlePreview() {
  if (!sdCardAvailable) {
    sdWebSendError(503, "SD card is not mounted");
    return;
  }

  String path = sdWebNormalizePath(sdWebServer.arg("path"));
  if (!path.length() || path == "/") {
    sdWebSendError(400, "Invalid image path");
    return;
  }

  String lower = path;
  lower.toLowerCase();
  if (!lower.endsWith(".bmp")) {
    sdWebSendError(400, "Only BMP images can be previewed");
    return;
  }

  sdWebBusy = true;
  File imageFile = SD.open(path, FILE_READ);
  if (!imageFile || imageFile.isDirectory()) {
    if (imageFile) imageFile.close();
    sdWebBusy = false;
    sdWebSendError(404, "BMP file not found");
    return;
  }

  sdWebServer.sendHeader("Cache-Control", "no-store");
  sdWebServer.sendHeader("X-Content-Type-Options", "nosniff");
  size_t sent = sdWebServer.streamFile(imageFile, "image/bmp");
  imageFile.close();
  sdWebBusy = false;

  Serial.printf("BMP preview: %s (%u bytes sent)\\n",
                path.c_str(), (unsigned int)sent);
}

static void sdWebHandleMkdir() {
  if(!sdCardAvailable){sdWebSendError(503,"SD card is not mounted");return;} String base=sdWebNormalizePath(sdWebServer.arg("path"));String name=sdWebServer.arg("name");
  if(!base.length()||!sdWebSafeName(name)){sdWebSendError(400,"Invalid folder name or path");return;} String target=base=="/"?"/"+name:base+"/"+name;
  sdWebBusy=true;bool ok=SD.mkdir(target);sdWebBusy=false;if(!ok){sdWebSendError(400,"Could not create folder; it may already exist");return;}sdWebServer.send(200,"application/json","{\"ok\":true}");
}
static void sdWebHandleDelete() {
  if(!sdCardAvailable){sdWebSendError(503,"SD card is not mounted");return;} String path=sdWebNormalizePath(sdWebServer.arg("path"));if(!path.length()||path=="/"){sdWebSendError(400,"Invalid path");return;}
  sdWebBusy=true;File f=SD.open(path);if(!f){sdWebBusy=false;sdWebSendError(404,"Item not found");return;}bool isDir=f.isDirectory();f.close();String lower=path;lower.toLowerCase();if(!isDir&&!lower.endsWith(".bmp")){sdWebBusy=false;sdWebSendError(400,"Only BMP files can be deleted");return;}bool ok=isDir?SD.rmdir(path):SD.remove(path);sdWebBusy=false;if(!ok){sdWebSendError(400,isDir?"Folder must be empty":"Could not delete file");return;}sdWebServer.send(200,"application/json","{\"ok\":true}");
}
static void sdWebHandleUploadStream() {
  HTTPUpload &u=sdWebServer.upload();
  if(u.status==UPLOAD_FILE_START){
    sdWebBusy=true;sdWebUploadFailed=false;sdWebUploadError="";sdWebUploadBytes=0;sdWebUploadPath="";
    if(!sdCardAvailable){sdWebUploadFailed=true;sdWebUploadError="SD card is not mounted";return;}
    String name=sdWebServer.arg("name"); if(!name.length())name=u.filename; if(!sdWebSafeName(name)){sdWebUploadFailed=true;sdWebUploadError="Invalid filename";return;}String lower=name;lower.toLowerCase();if(!lower.endsWith(".bmp")){sdWebUploadFailed=true;sdWebUploadError="Only BMP files are accepted";return;}
    String base=sdWebNormalizePath(sdWebServer.arg("path"));if(!base.length()){sdWebUploadFailed=true;sdWebUploadError="Invalid destination folder";return;}sdWebUploadPath=base=="/"?"/"+name:base+"/"+name;
    if(SD.exists(sdWebUploadPath)){sdWebUploadFailed=true;sdWebUploadError="A file with that name already exists";return;}
    sdWebUploadFile=SD.open(sdWebUploadPath,FILE_WRITE);if(!sdWebUploadFile){sdWebUploadFailed=true;sdWebUploadError="Could not create file on SD card";return;}
  } else if(u.status==UPLOAD_FILE_WRITE){
    if(!sdWebUploadFailed&&sdWebUploadFile){size_t n=sdWebUploadFile.write(u.buf,u.currentSize);sdWebUploadBytes+=n;if(n!=u.currentSize){sdWebUploadFailed=true;sdWebUploadError="SD write failed";}}
    // The upload callback runs in the main loop. Keep LVGL animation/timers alive,
    // but suppress touch while an SD operation is active.
    uint32_t now=millis();if(now-sdWebLastLvglService>=20){lv_tick_inc(now-lastLvglTickMillis);lastLvglTickMillis=now;sdWebLastLvglService=now;lv_timer_handler();}
  } else if(u.status==UPLOAD_FILE_END){
    if(sdWebUploadFile){sdWebUploadFile.flush();sdWebUploadFile.close();}
    if(!sdWebUploadFailed&&sdWebUploadPath.length()){File check=SD.open(sdWebUploadPath,FILE_READ);if(!check||check.size()<54){sdWebUploadFailed=true;sdWebUploadError="Uploaded file is too small to be a BMP";}else{uint8_t sig[2]={0,0};check.read(sig,2);if(sig[0]!='B'||sig[1]!='M'){sdWebUploadFailed=true;sdWebUploadError="File does not have a BMP signature";}check.close();}}
    if(sdWebUploadFailed&&sdWebUploadPath.length())SD.remove(sdWebUploadPath);
    sdWebBusy=false;
  } else if(u.status==UPLOAD_FILE_ABORTED){if(sdWebUploadFile)sdWebUploadFile.close();if(sdWebUploadPath.length())SD.remove(sdWebUploadPath);sdWebUploadFailed=true;sdWebUploadError="Upload was interrupted";sdWebBusy=false;}
}
static void sdWebHandleUploadResult() {
  if(!sdCardAvailable){sdWebSendError(503,"SD card is not mounted");return;}if(sdWebUploadFailed){sdWebSendError(400,sdWebUploadError.length()?sdWebUploadError:"Upload failed");return;}sdWebServer.send(200,"application/json","{\"ok\":true,\"bytes\":"+String(sdWebUploadBytes)+"}");
}

void sdWebManagerBegin() {
  sdWebLastLvglService = millis();
  WiFi.mode(WIFI_AP_STA); WiFi.setSleep(false);

  // The AP interface MAC may be all zeros until the AP interface has been
  // brought up. Start a temporary AP first, read its real MAC, then restart
  // with the final device-specific SSID.
  bool ap = WiFi.softAP("LPS-Init", SD_WEB_AP_PASSWORD);
  if (ap) {
    delay(100);
    String apMac = WiFi.softAPmacAddress();
    apMac.replace(":", "");
    apMac.toUpperCase();

    String macSuffix = apMac.length() >= 6
                         ? apMac.substring(apMac.length() - 6)
                         : apMac;

    if (macSuffix.length() != 6 || macSuffix == "000000") {
      Serial.print("ERROR: Invalid AP MAC address: ");
      Serial.println(WiFi.softAPmacAddress());
      WiFi.softAPdisconnect(false);
      Serial.println("ERROR: SD manager AP failed (could not read AP MAC)");
      return;
    }

    sdWebApSsid = String(SD_WEB_AP_PREFIX) + macSuffix;
    WiFi.softAPdisconnect(false);
    delay(100);
    ap = WiFi.softAP(sdWebApSsid.c_str(), SD_WEB_AP_PASSWORD);
  }

  Serial.println(ap ? "SD manager AP started" : "ERROR: SD manager AP failed");
  if (ap) {
    Serial.print("SD manager Wi-Fi: ");
    Serial.println(sdWebApSsid);
    Serial.print("AP MAC: ");
    Serial.println(WiFi.softAPmacAddress());
    Serial.print("Open http://");
    Serial.println(WiFi.softAPIP());
  }
  sdWebServer.on("/",HTTP_GET,sdWebHandleRoot);
  sdWebServer.on("/api/list",HTTP_GET,sdWebHandleList);
  sdWebServer.on("/api/preview",HTTP_GET,sdWebHandlePreview);
  sdWebServer.on("/api/mkdir",HTTP_POST,sdWebHandleMkdir);
  sdWebServer.on("/api/delete",HTTP_POST,sdWebHandleDelete);
  sdWebServer.on("/api/upload",HTTP_POST,sdWebHandleUploadResult,sdWebHandleUploadStream);
  sdWebServer.onNotFound([](){sdWebSendError(404,"Endpoint not found");});
  sdWebServer.begin();sdWebStarted=true;Serial.println("SD manager HTTP server started");
}
void sdWebManagerLoop() { if(sdWebStarted)sdWebServer.handleClient(); }
