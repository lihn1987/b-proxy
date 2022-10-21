import requests
url = "http://35.220.221.140:9980/#/"
# url = "https://www.baidu.com"
proxies = {
    'http':'127.0.0.1:1011',
    'https':'127.0.0.1:1011',
}
r = requests.get(url, proxies=proxies)
print(r.status_code)
print(r.text)