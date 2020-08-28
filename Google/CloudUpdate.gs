function doGet(d) {
  const FOLDER_NAME = 'CloudUpdate';
  const CONFIG_NAME = 'CloudUpdate.cfg';

  let result = '';
  let platform, mac, md5;

  try {
    platform = d.parameter.platform;
    mac = d.parameter.mac;
    md5 = d.parameter.md5;
    if ((platform !== undefined) && (mac !== undefined) && (md5 !== undefined)) {
      let file;

      file = openFile(FOLDER_NAME, CONFIG_NAME);
      if (file != null) {
        let firmwareName = findFirmware(file.getBlob().getDataAsString(), platform, mac, md5);

        if (firmwareName != null) {
          file = openFile(FOLDER_NAME, firmwareName);
          if (file != null) {
            file.setSharing(DriveApp.Access.ANYONE_WITH_LINK, DriveApp.Permission.VIEW);
            result = 'https://drive.google.com/uc?export=download&id=' + file.getId();
          }
        }
      }
    }
  } catch(e) {
    console.error('Exception: ' + e.message);
  }
  return ContentService.createTextOutput(result);
}

function openFile(folderName, fileName) {
  let folders;

  folders = DriveApp.getFoldersByName(folderName);
  if (folders.hasNext()) {
    let folder, files;

    folder = folders.next();
    files = folder.getFilesByName(fileName);
    if (files.hasNext()) {
      return files.next();
    }
  }
  return null;
}

function findFirmware(config, platform, mac, md5) {
  let lines = String(config).split('\r\n');

  for (let i = 0; i < lines.length; ++i) {
    let line = lines[i];

    if ((line != '') && (line[0] != ';')) {
      let data = String(line).split(' ');

      if (data.length >= 4) {
        if (data[0] == platform) {
          if (checkMAC(data[1], mac)) {
            if (data[2].toLowerCase() != md5.toLowerCase()) {
              return data[3];
            }
          }
        }
      }
    }
  }
  return null;
}

function checkMAC(mask, mac) {
  let result = false;
  let maskValue = String(mask).split(':');
  let macValue = String(mac).split(':');

  if ((maskValue.length == 6) && (macValue.length == 6)) {
    for (let i = 0; i < maskValue.length; ++i) {
      let maskOctet = parseInt('0x' + maskValue[i]);
      let macOctet = parseInt('0x' + macValue[i]);

      result = (macOctet & maskOctet) == macOctet;
      if (! result)
        break;
    }
  }
  return result;
}

/*
function compareMD5(fileName, md5) {
  const FOLDER_NAME = 'CloudUpdate';

  let fileMD5 = getMD5(FOLDER_NAME, fileName);

  return (fileMD5 == md5.toLowerCase());
}

function getMD5(folderName, fileName) {
  let result = '';
  let file = openFile(folderName, fileName);

  if (file != null) {
    let bytes = Utilities.computeDigest(Utilities.DigestAlgorithm.MD5, file.getBlob().getBytes());

    for (let i = 0; i < bytes.length; ++i) {
      let s;
      let b = bytes[i];

      if (b < 0)
        b += 256;
      s = b.toString(16);
      if (s.length == 1)
        s = '0' + s;
      result += s;
    }
    result = result.toLowerCase();
  }
  return result;
}
*/
