# Proposals

| Doc | Comment | 
| --- | --- |
| [CredsHolder Project Requirements Doc](creds-holder.md) |  |
| [CredsHolder Authentication High Level Design](creds-holder-auth.md) | <br/>PIN will unlock device<br/>OLED will be hidden inside case or User will receive PIN suggestion using Morse code via vibration<br/>Concrete UI is not chosen |
| [CredsHolder Storage High Level Design](creds-holder-storage.md) | <br/>Encrypted VeraCrypt volume on removable microSD card together with VeraCrypt portable applications<br/>PIN for CredsHolder unlock but custom complex password and non-default PIM for VeraCrypt volume encryption<br/>VeraCrypt volume will have FAT FS and contain SQLite DBs with application data |
