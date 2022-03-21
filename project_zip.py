from zipfile import *
import os

def zipFilesInDir(zipFile, dirName, flatten = False, filter = lambda n: True):
  # Iterate over all the files in directory
  for folderName, subfolders, filenames in os.walk(dirName):
    for filename in filenames:
      filePath = os.path.join(folderName, filename)
      if filter(filePath):
        # Add file to zip
        if flatten:
          zipFile.write(filePath, os.path.basename(filePath))
        else:
          zipFile.write(filePath)

def main():
  if not os.path.exists('./artifacts'):
    os.makedirs('./artifacts')
  with ZipFile('./artifacts/lsystem.zip', 'w') as z:
    zipFilesInDir(z, 'build', True, lambda name: 'img' not in name and 'png' not in name)
    zipFilesInDir(z, 'examples', False, lambda name: "ini" in name)
  
if __name__ == "__main__":
  main()