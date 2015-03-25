import SocketServer
import SimpleHTTPServer
import urllib
import urlparse
import re
import cgi

PORT = 1234

class Proxy(SimpleHTTPServer.SimpleHTTPRequestHandler):
	def do_GET(self):
                if "googlevideo" in self.path:
                        self.path = re.sub('^http://', 'https://', self.path)
		self.send_response(200)
		self.end_headers()
		self.copyfile(urllib.urlopen(self.path), self.wfile)
	def do_POST(self):
		length = int(self.headers.getheaders("Content-Length")[0])
		post_data = urlparse.parse_qs(self.rfile.read(length))
                self.copyfile(urllib.urlopen(self.path,urllib.urlencode(post_data)),self.wfile)

httpd = SocketServer.ForkingTCPServer(('', PORT), Proxy)
print "serving at port", PORT
httpd.serve_forever()

