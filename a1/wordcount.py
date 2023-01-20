filename = 'Assgn1_2_29.sh'
wordCount = 0

with open(filename, 'r') as asgnmntfile:
    content = asgnmntfile.read().replace('=', ' = ').replace('|', ' | ').replace(';', ' ; ').replace(',', ' , ').replace('<', ' < ').replace('>', ' > ')
    wordCount+=len(content.split())

print(f"Word count for {filename} : {wordCount}")