import subprocess

revision = (
    subprocess.check_output(["git", "describe", "--always", "--tags", "--dirty", "--long"])
    .strip()
    .decode("utf-8")
)
print("-DGIT_REV='\"%s\"'" % revision)