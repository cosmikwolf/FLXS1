import subprocess

revision = (
    subprocess.check_output(["git", "describe", "--tags", "--dirty", "--long"])
    .strip()
    .decode("utf-8")
)
print("-DGIT_REV='\"%s\"'" % revision)