tell application "Mail"
	set accs to name of every account
end tell
set unCnt to 0
repeat with theAcc in accs
	set theAccount to theAcc
	set theMailbox to "INBOX"
	tell application "Mail"
		set mailboxValue to mailbox theMailbox of account theAccount
		set unCnt to unCnt + the unread count of mailboxValue
	end tell
end repeat
return unCnt
