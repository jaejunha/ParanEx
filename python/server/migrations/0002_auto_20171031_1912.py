# -*- coding: utf-8 -*-
# Generated by Django 1.11.3 on 2017-10-31 10:12
from __future__ import unicode_literals

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('server', '0001_initial'),
    ]

    operations = [
        migrations.AlterUniqueTogether(
            name='subject',
            unique_together=set([('number', 'name')]),
        ),
    ]
