-------------------------------------------------------------------------
演習1: 解説
-------------------------------------------------------------------------

.. include:: ../header.rst

coreutilsの最新のコードの入手
------------------------------------------------------------------------
.. 実演

.. code-block:: console

    $ git clone git://git.sv.gnu.org/coreutils.git
    $ cd coreutils
    $ git submodule init
    $ git submodule update


glibcソースパッケージの入手、展開、合成、配置
------------------------------------------------------------------------
.. 実演

.. code-block:: console

	$ cd /tmp
	$ yumdownloader --source glibc
	# su 
	# yum-builddep glibc*.src.rpm
	...
	Is this ok [y/N]: y
	...
	# exit
	$ rpm -ivh coreutils-*.src.rpm
	$ cd ~/rpmbuild
	$ rpmbuild -bp SPECS/glibc.spec
	$ mv BUILD/glibc* ~/fedora

