FROM cequenser/sequencer-ci-linux:latest
MAINTAINER cequenser "cequenser@gmail.com"

RUN apt-get update && apt-get install -y openjdk-8-jdk openssh-server && sed -i 's|session required pam_loginuid.so|session optional pam_loginuid.so|g' /etc/pam.d/sshd && mkdir -p /var/run/sshd && adduser --quiet jenkins && echo "jenkins:jenkins" | chpasswd && mkdir /home/jenkins/.m2
COPY id_rsa.pub /home/jenkins/.ssh/id_rsa.pub
RUN chown -R jenkins:jenkins /home/jenkins/.m2/ && \
    chown -R jenkins:jenkins /home/jenkins/.ssh/

# Standard SSH port
EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]

WORKDIR /home/jenkins
