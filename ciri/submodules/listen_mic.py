import speech_recognition as sr
import sys
r = sr.Recognizer()
with sr.Microphone() as source:
	audio = r.listen(source)

try:
	sys.stdout.write(r.recognize_google(audio))
except LookupError:
	sys.stdout.write("--")
