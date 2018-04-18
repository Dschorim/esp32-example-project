const static char http_html_hdr[] =
    "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

const static char http_index_hml1[] = "<!DOCTYPE html>\n"
		"<html>\n"
		  "<head>\n"
		    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
		    "<style type=\"text/css\">\n"
		      "html, body, iframe { margin: 0; padding: 0; height: 100%; }\n"
		      "p {text-align: Center; font-size: 20pt; margin-bottom: 5px;}\n"
		      ".switch {position: relative;display: inline-block;width: 60px;height: 34px;}\n"
		      ".switch input {display:none;}\n"
		      ".toggle {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;\n"
		        "bottom: 0;background-color: #ccc;-webkit-transition: .4s;transition: .4s;}\n"
		      ".toggle:before {position: absolute;content: \"\";height: 26px;width: 26px;\n"
		        "left: 4px;bottom: 4px;background-color: white;-webkit-transition: .4s;transition: .4s;}\n"
		      "input:checked + .toggle {background-color: #2196F3;}\n"
		      "input:focus + .toggle {box-shadow: 0 0 1px #2196F3;}\n"
		      "input:checked + .toggle:before {-webkit-transform: translateX(26px);\n"
		          "-ms-transform: translateX(26px);transform: translateX(26px);}\n"
		      ".toggle.round {border-radius: 34px;}\n"
		      ".toggle.round:before {border-radius: 50%;}\n"
		      "div {width: auto;text-align: center;}\n"
		    "</style>\n"
		    "<title>ESP32 Control Center</title>\n"
		  "</head>\n"
		  "<body>\n"
		    "<p>Toggle OFF/ON</p>\n"
		      "<div>\n"
		        "<label class=\"switch\">\n"
		          "<form method=\"get\" action=\"/o\">\n"
                "<input type=\"checkbox\" onChange=\"form.submit()\" ";
const static char http_index_hml2_0[] = "";
const static char http_index_hml2_1[] = "checked";
const static char http_index_hml3[] = ">\n"
		            "<span class=\"toggle round\"></span>\n"
		          "</form>\n"
		        "</label>\n"
		      "</div>\n"
		    "<p>Toggle WebControl/AnalogControl</p>\n"
		    "<div>\n"
		      "<label class=\"switch\">\n"
		        "<form method=\"get\" action=\"/c\">\n"
		        "<input type=\"checkbox\" onChange=\"form.submit()\" ";
const static char http_index_hml4_1[] = "checked";
const static char http_index_hml4_0[] = "";
const static char http_index_hml5[] = ">\n"
		        "<span class=\"toggle round\"></span></form>\n"
		      "</label>\n"
		    "</div>\n"
		    "<p>Brightness of LED</p>\n"
		      "<div>\n"
		        "<div class=\"slidecontainer\">\n"
		          "<form method=\"get\">\n"
		            "<input type=\"range\" value=\"";
char http_index_hml6[] = "190";
const static char http_index_hml7[] = "\" min=\"190\" max=\"255\" name=\"b\" onChange=\"form.submit()\">\n"
		          "</form>\n"
		        "</div>\n"
		      "<div>\n"
		  "</body>\n"
		"</html>";
