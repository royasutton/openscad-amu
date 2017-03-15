ext := svg

flow.$(ext): flow.dot Makefile
	dot \
		-Grankdir="LR" \
		-Gratio=fill \
		-Gsize="8,6" \
		\
		-v -T$(ext) -o $@ $<

flow_intro.$(ext): flow_intro.dot Makefile
	dot \
		-Grankdir="LR" \
		-Gratio=fill \
		\
		-v -T$(ext) -o $@ $<

embedding.$(ext): embedding.dot Makefile
	dot \
		-Gratio=fill \
		-Gsize="6,4" \
		\
		-v -T$(ext) -o $@ $<

all: \
	flow.$(ext) \
	flow_intro.$(ext) \
	embedding.$(ext)
