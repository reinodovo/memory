def contents(path):
    with open(path, 'r') as file:
        return file.read()
    
template = contents('./manual/template.html')
rules = contents('./manual/rules.html')
manual = template.replace("{{rules}}", rules)
with open('./manual/pages/page1.html', 'w') as file:
    file.write(manual)