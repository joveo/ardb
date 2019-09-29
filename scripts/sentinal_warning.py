
import slack
import sys
client = slack.WebClient(token='xoxb-144300912372-t6pGwCLFSvbEOlWzOfSJ8Nw8')
event = sys.argv[1]
desc = sys.argv[2]
stats = sys.argv[3]
print (event + " " + desc)
response = client.chat_postMessage(
        channel='#rocksdb-debug',
        text=event + ": " + desc + "\n" + "** Stats **\n" + stats)
assert response["ok"]
